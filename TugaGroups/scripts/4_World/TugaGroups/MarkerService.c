class TugaGroupsMarkerService
{
    private ref array<ref TugaGroupsAdminMarker> m_AdminMarkers;
    private ref map<string, ref array<ref TugaGroupsGroupMarker>> m_GroupMarkers;
    private ref map<string, ref array<ref TugaGroupsPrivateMarker>> m_PrivateMarkers;
    private ref map<string, ref array<ref TugaGroupsPingMarker>> m_GroupPings;
    private ref map<string, int> m_LastPingTimes;
    private ref TugaGroupsSettings m_Settings;

    void TugaGroupsMarkerService(TugaGroupsSettings settings)
    {
        m_AdminMarkers = new array<ref TugaGroupsAdminMarker>();
        m_GroupMarkers = new map<string, ref array<ref TugaGroupsGroupMarker>>();
        m_PrivateMarkers = new map<string, ref array<ref TugaGroupsPrivateMarker>>();
        m_GroupPings = new map<string, ref array<ref TugaGroupsPingMarker>>();
        m_LastPingTimes = new map<string, int>();
        m_Settings = settings;
    }

    string AdminMarkerPath()
    {
        return "$profile:TugaGroups/AdminMarkers.json";
    }

    void LoadAdminMarkers()
    {
        string path = AdminMarkerPath();
        EnsureAdminMarkersFile(path);
        JsonFileLoader<array<ref TugaGroupsAdminMarker>>.JsonLoadFile(path, m_AdminMarkers);
    }

    void EnsureAdminMarkersFile(string path)
    {
        if (FileExist(path))
        {
            return;
        }
        MakeDirectory("$profile:TugaGroups");
        array<ref TugaGroupsAdminMarker> defaults = new array<ref TugaGroupsAdminMarker>();
        JsonFileLoader<array<ref TugaGroupsAdminMarker>>.JsonSaveFile(path, defaults);
    }

    void InsertGroupMarker(TugaGroupsGroupMarker marker)
    {
        if (!marker || marker.OwnerGroupId == string.Empty)
        {
            return;
        }

        array<ref TugaGroupsGroupMarker> markers;
        if (!m_GroupMarkers.Find(marker.OwnerGroupId, markers))
        {
            markers = new array<ref TugaGroupsGroupMarker>();
            m_GroupMarkers.Set(marker.OwnerGroupId, markers);
        }
        markers.Insert(marker);
    }

    void InsertPrivateMarker(TugaGroupsPrivateMarker marker)
    {
        if (!marker || marker.OwnerId == string.Empty)
        {
            return;
        }

        array<ref TugaGroupsPrivateMarker> markers;
        if (!m_PrivateMarkers.Find(marker.OwnerId, markers))
        {
            markers = new array<ref TugaGroupsPrivateMarker>();
            m_PrivateMarkers.Set(marker.OwnerId, markers);
        }
        markers.Insert(marker);
    }

    bool IsIconAllowed(string icon)
    {
        foreach (string allowed : m_Settings.IconWhitelist)
        {
            if (allowed == icon)
            {
                return true;
            }
        }
        return false;
    }

    bool AddGroupMarker(TugaGroupsGroup group, PlayerIdentity actor, vector position, string name, string icon, int color, bool renderAs3d)
    {
        if (!group || !actor)
        {
            return false;
        }

        TugaGroupsGroupMember member = group.GetMember(actor.GetId());
        if (!member || !m_Settings.IsRankAllowed(member.Rank, m_Settings.AllowedRanks.AddGroupMarker))
        {
            return false;
        }

        string sanitized = m_Settings.SanitizeLabel(name);
        if (sanitized == string.Empty)
        {
            return false;
        }

        if (!IsIconAllowed(icon))
        {
            return false;
        }

        array<ref TugaGroupsGroupMarker> markers;
        if (!m_GroupMarkers.Find(group.GroupId, markers))
        {
            markers = new array<ref TugaGroupsGroupMarker>();
            m_GroupMarkers.Set(group.GroupId, markers);
        }

        if (markers.Count() >= m_Settings.MaxStaticMarkersPerGroup)
        {
            return false;
        }

        TugaGroupsGroupMarker marker = new TugaGroupsGroupMarker();
        marker.Position = position;
        marker.Name = sanitized;
        marker.Icon = icon;
        marker.Color = color;
        marker.OwnerGroupId = group.GroupId;
        marker.OwnerId = actor.GetId();
        marker.CreatedAt = GetGame().GetTime();
        marker.RenderAs3D = renderAs3d;

        markers.Insert(marker);
        return true;
    }

    bool AddPrivateMarker(PlayerIdentity actor, vector position, string name, string icon, int color, bool renderAs3d)
    {
        if (!actor)
        {
            return false;
        }

        string sanitized = m_Settings.SanitizeLabel(name);
        if (sanitized == string.Empty)
        {
            return false;
        }

        if (!IsIconAllowed(icon))
        {
            return false;
        }

        array<ref TugaGroupsPrivateMarker> markers;
        if (!m_PrivateMarkers.Find(actor.GetId(), markers))
        {
            markers = new array<ref TugaGroupsPrivateMarker>();
            m_PrivateMarkers.Set(actor.GetId(), markers);
        }

        if (markers.Count() >= m_Settings.MaxStaticMarkersPerGroup)
        {
            return false;
        }

        TugaGroupsPrivateMarker marker = new TugaGroupsPrivateMarker();
        marker.Position = position;
        marker.Name = sanitized;
        marker.Icon = icon;
        marker.Color = color;
        marker.OwnerId = actor.GetId();
        marker.CreatedAt = GetGame().GetTime();
        marker.RenderAs3D = renderAs3d;

        markers.Insert(marker);
        return true;
    }

    bool RemoveGroupMarker(TugaGroupsGroup group, PlayerIdentity actor, int index)
    {
        if (!group || !actor)
        {
            return false;
        }

        TugaGroupsGroupMember member = group.GetMember(actor.GetId());
        if (!member || !m_Settings.IsRankAllowed(member.Rank, m_Settings.AllowedRanks.RemoveGroupMarker))
        {
            return false;
        }

        array<ref TugaGroupsGroupMarker> markers;
        if (!m_GroupMarkers.Find(group.GroupId, markers))
        {
            return false;
        }

        if (index < 0 || index >= markers.Count())
        {
            return false;
        }

        markers.Remove(index);
        return true;
    }

    bool RemovePrivateMarker(PlayerIdentity actor, int index)
    {
        if (!actor)
        {
            return false;
        }

        array<ref TugaGroupsPrivateMarker> markers;
        if (!m_PrivateMarkers.Find(actor.GetId(), markers))
        {
            return false;
        }

        if (index < 0 || index >= markers.Count())
        {
            return false;
        }

        markers.Remove(index);
        return true;
    }

    bool AddPing(TugaGroupsGroup group, PlayerIdentity actor, vector position, string icon, float size)
    {
        if (!group || !actor)
        {
            return false;
        }

        TugaGroupsGroupMember member = group.GetMember(actor.GetId());
        if (!member || !m_Settings.IsRankAllowed(member.Rank, m_Settings.AllowedRanks.Ping))
        {
            return false;
        }

        int lastPing;
        int now = GetGame().GetTime();
        if (m_LastPingTimes.Find(actor.GetId(), lastPing))
        {
            if ((now - lastPing) < (m_Settings.PingCooldownSeconds * 1000))
            {
                return false;
            }
        }

        array<ref TugaGroupsPingMarker> pings;
        if (!m_GroupPings.Find(group.GroupId, pings))
        {
            pings = new array<ref TugaGroupsPingMarker>();
            m_GroupPings.Set(group.GroupId, pings);
        }

        if (pings.Count() >= m_Settings.MaxActivePingsPerGroup)
        {
            return false;
        }

        TugaGroupsPingMarker ping = new TugaGroupsPingMarker();
        ping.Position = position;
        ping.Name = actor.GetName();
        ping.Icon = icon;
        ping.Color = 0xFF00FFFF;
        ping.OwnerGroupId = group.GroupId;
        ping.OwnerId = actor.GetId();
        ping.CreatedAt = now;
        ping.RenderAs3D = false;
        ping.Size = size;

        pings.Insert(ping);
        m_LastPingTimes.Set(actor.GetId(), now);
        return true;
    }

    void CleanupExpiredPings()
    {
        int now = GetGame().GetTime();
        foreach (string groupId, array<ref TugaGroupsPingMarker> pings : m_GroupPings)
        {
            for (int i = pings.Count() - 1; i >= 0; i--)
            {
                if ((now - pings[i].CreatedAt) > (m_Settings.PingTtlSeconds * 1000))
                {
                    pings.Remove(i);
                }
            }
        }
    }

    array<ref TugaGroupsAdminMarker> GetAdminMarkers()
    {
        return m_AdminMarkers;
    }

    void SetAdminMarkers(array<ref TugaGroupsAdminMarker> markers)
    {
        m_AdminMarkers = markers;
    }

    array<ref TugaGroupsGroupMarker> GetGroupMarkers(string groupId)
    {
        array<ref TugaGroupsGroupMarker> markers;
        if (!m_GroupMarkers.Find(groupId, markers))
        {
            return new array<ref TugaGroupsGroupMarker>();
        }
        return markers;
    }

    array<ref TugaGroupsPrivateMarker> GetPrivateMarkers(string playerId)
    {
        array<ref TugaGroupsPrivateMarker> markers;
        if (!m_PrivateMarkers.Find(playerId, markers))
        {
            return new array<ref TugaGroupsPrivateMarker>();
        }
        return markers;
    }

    array<ref TugaGroupsPingMarker> GetPings(string groupId)
    {
        array<ref TugaGroupsPingMarker> pings;
        if (!m_GroupPings.Find(groupId, pings))
        {
            return new array<ref TugaGroupsPingMarker>();
        }
        return pings;
    }
};
