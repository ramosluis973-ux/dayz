modded class MissionServer
{
    private ref TugaGroupsSettings m_TugaGroupsSettings;
    private ref TugaGroupsGroupService m_GroupService;
    private ref TugaGroupsMarkerService m_MarkerService;
    private ref TugaGroupsServerSettings m_ServerSettings;

    override void OnInit()
    {
        super.OnInit();
        m_ServerSettings = TugaGroupsServerSettings.Load();
        m_TugaGroupsSettings = new TugaGroupsSettings();
        m_TugaGroupsSettings.MaxGroupSize = m_ServerSettings.MaxGroupSize;
        m_TugaGroupsSettings.PingDistance.ShowDistance = m_ServerSettings.ShowPingDistance;
        m_GroupService = new TugaGroupsGroupService(m_TugaGroupsSettings);
        m_MarkerService = new TugaGroupsMarkerService(m_TugaGroupsSettings);
        m_MarkerService.LoadAdminMarkers();
        RegisterTugaGroupsRPCs();
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        if (m_MarkerService)
        {
            m_MarkerService.CleanupExpiredPings();
        }
    }

    override void OnClientReadyEvent(PlayerIdentity identity, PlayerBase player)
    {
        super.OnClientReadyEvent(identity, player);
        SendInitialSync(identity);
    }

    void RegisterTugaGroupsRPCs()
    {
        GetRPCManager().AddRPC("TugaGroups", "HandleCreateGroup", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleInvite", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleAccept", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleLeave", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleKick", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandlePromote", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleDemote", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleTransferLeader", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleDisband", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleAddGroupMarker", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleRemoveGroupMarker", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleAddPrivateMarker", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandleRemovePrivateMarker", this, SingleplayerExecutionType.Server);
        GetRPCManager().AddRPC("TugaGroups", "HandlePing", this, SingleplayerExecutionType.Server);
    }

    void SendInitialSync(PlayerIdentity identity)
    {
        if (!identity)
        {
            return;
        }

        GetRPCManager().SendRPC("TugaGroups", "SyncSettings", new Param1<TugaGroupsSettings>(m_TugaGroupsSettings), true, identity);

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(identity.GetId());
        array<ref TugaGroupsGroupMarker> groupMarkers;
        array<ref TugaGroupsPingMarker> groupPings;
        array<ref TugaGroupsPrivateMarker> privateMarkers;
        if (group)
        {
            groupMarkers = m_MarkerService.GetGroupMarkers(group.GroupId);
            groupPings = m_MarkerService.GetPings(group.GroupId);
            GetRPCManager().SendRPC("TugaGroups", "SyncGroupState", new Param1<TugaGroupsGroup>(group), true, identity);
        }
        else
        {
            groupMarkers = new array<ref TugaGroupsGroupMarker>();
            groupPings = new array<ref TugaGroupsPingMarker>();
            GetRPCManager().SendRPC("TugaGroups", "SyncGroupState", new Param1<TugaGroupsGroup>(null), true, identity);
        }
        privateMarkers = m_MarkerService.GetPrivateMarkers(identity.GetId());

        Param4<
            array<ref TugaGroupsAdminMarker>,
            array<ref TugaGroupsGroupMarker>,
            array<ref TugaGroupsPingMarker>,
            array<ref TugaGroupsPrivateMarker> > markerSync =
            new Param4<
                array<ref TugaGroupsAdminMarker>,
                array<ref TugaGroupsGroupMarker>,
                array<ref TugaGroupsPingMarker>,
                array<ref TugaGroupsPrivateMarker> >(
                m_MarkerService.GetAdminMarkers(),
                groupMarkers,
                groupPings,
                privateMarkers
            );

        GetRPCManager().SendRPC(
            "TugaGroups",
            "SyncMarkers",
            markerSync,
            true,
            identity
        );
    }

    PlayerBase FindPlayerById(string playerId)
    {
        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (player && player.GetIdentity() && player.GetIdentity().GetId() == playerId)
            {
                return player;
            }
        }
        return null;
    }

    void SyncGroupToMembers(TugaGroupsGroup group)
    {
        if (!group)
        {
            return;
        }

        foreach (TugaGroupsGroupMember member : group.Members)
        {
            PlayerBase player = FindPlayerById(member.Id);
            if (player && player.GetIdentity())
            {
                SendInitialSync(player.GetIdentity());
            }
        }
    }

    void HandleInvite(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        PlayerBase targetPlayer = FindPlayerById(data.param1);
        if (!targetPlayer || !targetPlayer.GetIdentity())
        {
            return;
        }

        if (m_GroupService.InvitePlayer(sender, targetPlayer.GetIdentity()))
        {
            GetRPCManager().SendRPC("TugaGroups", "InviteReceived", new Param1<string>(sender.GetId()), true, targetPlayer.GetIdentity());
            SendNotification(targetPlayer.GetIdentity(), "You have been invited to a group. Press Ctrl+J to accept.");
        }
        else
        {
            SendNotification(sender, "Invite failed.");
        }
    }

    void HandleCreateGroup(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param2<string, string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        string name = m_TugaGroupsSettings.SanitizeLabel(data.param1);
        string tag = m_TugaGroupsSettings.SanitizeLabel(data.param2);
        if (name == string.Empty)
        {
            SendNotification(sender, "Group name is invalid.");
            return;
        }
        if (tag.Length() < 3)
        {
            SendNotification(sender, "Group tag must be at least 3 characters.");
            return;
        }
        if (m_GroupService.IsGroupNameTaken(name))
        {
            SendNotification(sender, "Group name is already taken.");
            return;
        }
        if (m_GroupService.IsGroupTagTaken(tag))
        {
            SendNotification(sender, "Group tag is already taken.");
            return;
        }

        if (m_GroupService.CreateGroup(sender, name, tag))
        {
            SendInitialSync(sender);
        }
        else
        {
            SendNotification(sender, "Group creation failed.");
        }
    }

    void HandleAccept(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        if (m_GroupService.AcceptInvite(sender))
        {
            TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
            SyncGroupToMembers(group);
        }
        else
        {
            SendNotification(sender, "Invite expired or invalid.");
        }
    }

    void HandleLeave(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_GroupService.LeaveGroup(sender))
        {
            if (group)
            {
                SyncGroupToMembers(group);
            }
            SendInitialSync(sender);
        }
        else
        {
            SendNotification(sender, "Leave failed.");
        }
    }

    void HandleKick(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_GroupService.KickMember(sender, data.param1))
        {
            SyncGroupToMembers(group);
            PlayerBase kickedPlayer = FindPlayerById(data.param1);
            if (kickedPlayer && kickedPlayer.GetIdentity())
            {
                SendInitialSync(kickedPlayer.GetIdentity());
            }
        }
        else
        {
            SendNotification(sender, "Kick failed.");
        }
    }

    void HandlePromote(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_GroupService.PromoteMember(sender, data.param1))
        {
            SyncGroupToMembers(group);
        }
        else
        {
            SendNotification(sender, "Promote failed.");
        }
    }

    void HandleDemote(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_GroupService.DemoteMember(sender, data.param1))
        {
            SyncGroupToMembers(group);
        }
        else
        {
            SendNotification(sender, "Demote failed.");
        }
    }

    void HandleTransferLeader(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_GroupService.TransferLeader(sender, data.param1))
        {
            SyncGroupToMembers(group);
        }
        else
        {
            SendNotification(sender, "Transfer failed.");
        }
    }

    void HandleDisband(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        if (m_GroupService.DisbandGroup(sender))
        {
            SendInitialSync(sender);
        }
        else
        {
            SendNotification(sender, "Disband failed.");
        }
    }

    void SendNotification(PlayerIdentity identity, string message)
    {
        if (!identity || message == string.Empty)
        {
            return;
        }
        NotificationSystem.SendNotificationToPlayerIdentity(identity, 5, "TugaGroups", message, "");
    }

    void HandleAddGroupMarker(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param5<vector, string, string, int, bool> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_MarkerService.AddGroupMarker(group, sender, data.param1, data.param2, data.param3, data.param4, data.param5))
        {
            SyncGroupToMembers(group);
        }
    }

    void HandleAddPrivateMarker(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param5<vector, string, string, int, bool> data;
        if (!ctx.Read(data))
        {
            return;
        }

        if (m_MarkerService.AddPrivateMarker(sender, data.param1, data.param2, data.param3, data.param4, data.param5))
        {
            SendInitialSync(sender);
        }
    }

    void HandleRemoveGroupMarker(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<int> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_MarkerService.RemoveGroupMarker(group, sender, data.param1))
        {
            SyncGroupToMembers(group);
        }
    }

    void HandleRemovePrivateMarker(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param1<int> data;
        if (!ctx.Read(data))
        {
            return;
        }

        if (m_MarkerService.RemovePrivateMarker(sender, data.param1))
        {
            SendInitialSync(sender);
        }
    }

    void HandlePing(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Server || !sender)
        {
            return;
        }

        Param3<vector, string, float> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsGroup group = m_GroupService.GetGroupForPlayer(sender.GetId());
        if (m_MarkerService.AddPing(group, sender, data.param1, data.param2, data.param3))
        {
            SyncGroupToMembers(group);
        }
    }
};
