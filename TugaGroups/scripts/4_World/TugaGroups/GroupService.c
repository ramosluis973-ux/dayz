class TugaGroupsGroupService
{
    private ref map<string, ref TugaGroupsGroup> m_Groups;
    private ref map<string, string> m_PlayerGroups;
    private ref map<string, string> m_PendingInvites;
    private ref map<string, int> m_PendingInviteTimes;
    private ref map<string, int> m_LastInviteTimes;
    private ref map<string, string> m_GroupTags;
    private ref TugaGroupsSettings m_Settings;
    private string m_GroupSnapshotPath;

    void TugaGroupsGroupService(TugaGroupsSettings settings)
    {
        m_Groups = new map<string, ref TugaGroupsGroup>();
        m_PlayerGroups = new map<string, string>();
        m_PendingInvites = new map<string, string>();
        m_PendingInviteTimes = new map<string, int>();
        m_LastInviteTimes = new map<string, int>();
        m_GroupTags = new map<string, string>();
        m_Settings = settings;
        m_GroupSnapshotPath = "$profile:TugaGroups/Groups.json";
        SaveGroupSnapshot();
    }

    TugaGroupsGroup GetGroupForPlayer(string playerId)
    {
        string groupId;
        if (m_PlayerGroups.Find(playerId, groupId))
        {
            return m_Groups.Get(groupId);
        }
        return null;
    }

    string GetGroupIdForPlayer(string playerId)
    {
        string groupId;
        if (m_PlayerGroups.Find(playerId, groupId))
        {
            return groupId;
        }
        return string.Empty;
    }

    bool IsPlayerInvited(string playerId)
    {
        string groupId;
        return m_PendingInvites.Find(playerId, groupId);
    }

    string GetInviteGroupId(string playerId)
    {
        string groupId;
        if (m_PendingInvites.Find(playerId, groupId))
        {
            return groupId;
        }
        return string.Empty;
    }

    void ClearInvite(string playerId)
    {
        m_PendingInvites.Remove(playerId);
        m_PendingInviteTimes.Remove(playerId);
    }

    bool IsGroupNameTaken(string name)
    {
        foreach (string key, TugaGroupsGroup group : m_Groups)
        {
            if (group.Name == name)
            {
                return true;
            }
        }
        return false;
    }

    bool IsGroupTagTaken(string tag)
    {
        foreach (string key, string existingTag : m_GroupTags)
        {
            if (existingTag == tag)
            {
                return true;
            }
        }
        return false;
    }

    bool CreateGroup(PlayerIdentity identity, string name, string tag)
    {
        if (!identity)
        {
            return false;
        }

        if (m_PlayerGroups.Contains(identity.GetId()))
        {
            return false;
        }

        string sanitizedName = m_Settings.SanitizeLabel(name);
        string sanitizedTag = m_Settings.SanitizeLabel(tag);
        if (sanitizedName == string.Empty || sanitizedTag.Length() < 3)
        {
            return false;
        }

        if (IsGroupNameTaken(sanitizedName) || IsGroupTagTaken(sanitizedTag))
        {
            return false;
        }

        string groupId = identity.GetId();
        if (m_Groups.Contains(groupId))
        {
            return false;
        }

        TugaGroupsGroup group = new TugaGroupsGroup();
        group.GroupId = groupId;
        group.Name = sanitizedName;
        group.Tag = sanitizedTag;
        group.LeaderId = identity.GetId();
        group.Members.Insert(new TugaGroupsGroupMember(identity.GetId(), identity.GetName(), TugaGroupsGroupRank.Leader));

        m_Groups.Set(groupId, group);
        m_PlayerGroups.Set(identity.GetId(), groupId);
        m_GroupTags.Set(groupId, group.Tag);
        SaveGroupSnapshot();
        return true;
    }

    bool CanInvite(TugaGroupsGroup group, PlayerIdentity inviter)
    {
        TugaGroupsGroupMember member = group.GetMember(inviter.GetId());
        if (!member)
        {
            return false;
        }

        if (!m_Settings.IsRankAllowed(member.Rank, m_Settings.AllowedRanks.Invite))
        {
            return false;
        }

        int lastInvite;
        int now = GetGame().GetTime();
        if (m_LastInviteTimes.Find(inviter.GetId(), lastInvite))
        {
            if ((now - lastInvite) < (m_Settings.InviteCooldownSeconds * 1000))
            {
                return false;
            }
        }

        if (group.GetMemberCount() >= m_Settings.MaxGroupSize)
        {
            return false;
        }

        return true;
    }

    bool InvitePlayer(PlayerIdentity inviter, PlayerIdentity target)
    {
        if (!inviter || !target)
        {
            return false;
        }

        if (m_PlayerGroups.Contains(target.GetId()))
        {
            return false;
        }

        TugaGroupsGroup group = GetGroupForPlayer(inviter.GetId());
        if (!group)
        {
            return false;
        }

        if (!CanInvite(group, inviter))
        {
            return false;
        }

        m_PendingInvites.Set(target.GetId(), group.GroupId);
        m_PendingInviteTimes.Set(target.GetId(), GetGame().GetTime());
        m_LastInviteTimes.Set(inviter.GetId(), GetGame().GetTime());
        SaveGroupSnapshot();
        return true;
    }

    bool AcceptInvite(PlayerIdentity target)
    {
        if (!target)
        {
            return false;
        }

        string groupId;
        if (!m_PendingInvites.Find(target.GetId(), groupId))
        {
            return false;
        }

        int inviteTime;
        if (!m_PendingInviteTimes.Find(target.GetId(), inviteTime) || (GetGame().GetTime() - inviteTime) > 30000)
        {
            ClearInvite(target.GetId());
            return false;
        }

        TugaGroupsGroup group = m_Groups.Get(groupId);
        if (!group)
        {
            m_PendingInvites.Remove(target.GetId());
            return false;
        }

        if (group.GetMemberCount() >= m_Settings.MaxGroupSize)
        {
            return false;
        }

        group.Members.Insert(new TugaGroupsGroupMember(target.GetId(), target.GetName(), TugaGroupsGroupRank.Member));
        m_PlayerGroups.Set(target.GetId(), groupId);
        ClearInvite(target.GetId());
        SaveGroupSnapshot();
        return true;
    }

    bool LeaveGroup(PlayerIdentity player)
    {
        if (!player)
        {
            return false;
        }

        TugaGroupsGroup group = GetGroupForPlayer(player.GetId());
        if (!group)
        {
            return false;
        }

        bool wasLeader = (group.LeaderId == player.GetId());
        group.RemoveMember(player.GetId());
        m_PlayerGroups.Remove(player.GetId());

        if (group.GetMemberCount() == 0)
        {
            m_Groups.Remove(group.GroupId);
            m_GroupTags.Remove(group.GroupId);
            SaveGroupSnapshot();
            return true;
        }

        if (wasLeader)
        {
            TugaGroupsGroupMember newLeader = group.Members.Get(0);
            newLeader.Rank = TugaGroupsGroupRank.Leader;
            group.LeaderId = newLeader.Id;
        }

        SaveGroupSnapshot();
        return true;
    }

    bool KickMember(PlayerIdentity actor, string targetId)
    {
        TugaGroupsGroup group = GetGroupForPlayer(actor.GetId());
        if (!group)
        {
            return false;
        }

        TugaGroupsGroupMember actorMember = group.GetMember(actor.GetId());
        if (!actorMember || !m_Settings.IsRankAllowed(actorMember.Rank, m_Settings.AllowedRanks.Kick))
        {
            return false;
        }

        TugaGroupsGroupMember targetMember = group.GetMember(targetId);
        if (!targetMember)
        {
            return false;
        }

        if (targetMember.Rank == TugaGroupsGroupRank.Leader)
        {
            return false;
        }

        if (actorMember.Rank == TugaGroupsGroupRank.Officer && targetMember.Rank >= TugaGroupsGroupRank.Officer)
        {
            return false;
        }

        group.RemoveMember(targetId);
        m_PlayerGroups.Remove(targetId);
        SaveGroupSnapshot();
        return true;
    }

    bool PromoteMember(PlayerIdentity actor, string targetId)
    {
        TugaGroupsGroup group = GetGroupForPlayer(actor.GetId());
        if (!group)
        {
            return false;
        }

        TugaGroupsGroupMember actorMember = group.GetMember(actor.GetId());
        if (!actorMember || !m_Settings.IsRankAllowed(actorMember.Rank, m_Settings.AllowedRanks.Promote))
        {
            return false;
        }

        TugaGroupsGroupMember targetMember = group.GetMember(targetId);
        if (!targetMember || targetMember.Rank >= TugaGroupsGroupRank.Officer)
        {
            return false;
        }

        targetMember.Rank = targetMember.Rank + 1;
        SaveGroupSnapshot();
        return true;
    }

    bool DemoteMember(PlayerIdentity actor, string targetId)
    {
        TugaGroupsGroup group = GetGroupForPlayer(actor.GetId());
        if (!group)
        {
            return false;
        }

        TugaGroupsGroupMember actorMember = group.GetMember(actor.GetId());
        if (!actorMember || !m_Settings.IsRankAllowed(actorMember.Rank, m_Settings.AllowedRanks.Demote))
        {
            return false;
        }

        TugaGroupsGroupMember targetMember = group.GetMember(targetId);
        if (!targetMember || targetMember.Rank <= TugaGroupsGroupRank.Recruit)
        {
            return false;
        }

        targetMember.Rank = targetMember.Rank - 1;
        SaveGroupSnapshot();
        return true;
    }

    bool TransferLeader(PlayerIdentity actor, string targetId)
    {
        TugaGroupsGroup group = GetGroupForPlayer(actor.GetId());
        if (!group)
        {
            return false;
        }

        TugaGroupsGroupMember actorMember = group.GetMember(actor.GetId());
        if (!actorMember || !m_Settings.IsRankAllowed(actorMember.Rank, m_Settings.AllowedRanks.TransferLeader))
        {
            return false;
        }

        TugaGroupsGroupMember targetMember = group.GetMember(targetId);
        if (!targetMember)
        {
            return false;
        }

        actorMember.Rank = TugaGroupsGroupRank.Officer;
        targetMember.Rank = TugaGroupsGroupRank.Leader;
        group.LeaderId = targetMember.Id;
        SaveGroupSnapshot();
        return true;
    }

    bool DisbandGroup(PlayerIdentity actor)
    {
        TugaGroupsGroup group = GetGroupForPlayer(actor.GetId());
        if (!group)
        {
            return false;
        }

        if (group.LeaderId != actor.GetId())
        {
            return false;
        }

        foreach (TugaGroupsGroupMember member : group.Members)
        {
            m_PlayerGroups.Remove(member.Id);
        }

        m_Groups.Remove(group.GroupId);
        m_GroupTags.Remove(group.GroupId);
        SaveGroupSnapshot();
        return true;
    }

    array<ref TugaGroupsGroup> GetGroups()
    {
        array<ref TugaGroupsGroup> groups = new array<ref TugaGroupsGroup>();
        foreach (string key, TugaGroupsGroup group : m_Groups)
        {
            groups.Insert(group);
        }
        return groups;
    }

    void SaveGroupSnapshot()
    {
        MakeDirectory("$profile:TugaGroups");
        array<ref TugaGroupsGroup> groups = GetGroups();
        JsonFileLoader<array<ref TugaGroupsGroup> >.JsonSaveFile(m_GroupSnapshotPath, groups);
    }
};
