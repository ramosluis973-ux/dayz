class TugaGroupsClientManager
{
    private static ref TugaGroupsClientManager s_Instance;
    ref TugaGroupsClientState State;
    ref TugaGroupsGroupHudView GroupHud;
    ref TugaGroupsNameplate Nameplate;
    ref TugaGroupsCompassHud CompassHud;

    void TugaGroupsClientManager()
    {
        State = new TugaGroupsClientState();
        GroupHud = new TugaGroupsGroupHudView();
        Nameplate = new TugaGroupsNameplate();
        CompassHud = new TugaGroupsCompassHud();
    }

    static TugaGroupsClientManager Get()
    {
        if (!s_Instance)
        {
            s_Instance = new TugaGroupsClientManager();
        }
        return s_Instance;
    }

    void SendInvite(string targetId)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleInvite", new Param1<string>(targetId), true, null);
    }

    void SendCreateGroup(string name, string tag)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleCreateGroup", new Param2<string, string>(name, tag), true, null);
    }

    void SendAccept()
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleAccept", null, true, null);
    }

    void SendLeave()
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleLeave", null, true, null);
    }

    void SendKick(string targetId)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleKick", new Param1<string>(targetId), true, null);
    }

    void SendPromote(string targetId)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandlePromote", new Param1<string>(targetId), true, null);
    }

    void SendDemote(string targetId)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleDemote", new Param1<string>(targetId), true, null);
    }

    void SendTransferLeader(string targetId)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleTransferLeader", new Param1<string>(targetId), true, null);
    }

    void SendDisband()
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleDisband", null, true, null);
    }

    void SendAddGroupMarker(vector position, string name, string icon, int color)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleAddGroupMarker", new Param5<vector, string, string, int, bool>(position, name, icon, color, false), true, null);
    }

    void SendAddGroupMarker(vector position, string name, string icon, int color, bool renderAs3d)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleAddGroupMarker", new Param5<vector, string, string, int, bool>(position, name, icon, color, renderAs3d), true, null);
    }

    void SendRemoveGroupMarker(int index)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleRemoveGroupMarker", new Param1<int>(index), true, null);
    }

    void SendAddPrivateMarker(vector position, string name, string icon, int color, bool renderAs3d)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleAddPrivateMarker", new Param5<vector, string, string, int, bool>(position, name, icon, color, renderAs3d), true, null);
    }

    void SendRemovePrivateMarker(int index)
    {
        GetRPCManager().SendRPC("TugaGroups", "HandleRemovePrivateMarker", new Param1<int>(index), true, null);
    }

    void SendPing(vector position)
    {
        string icon = State.PingIcon;
        if (icon == string.Empty)
        {
            icon = "Ping";
        }
        GetRPCManager().SendRPC("TugaGroups", "HandlePing", new Param3<vector, string, float>(position, icon, State.PingSize), true, null);
    }
};

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();
        TugaGroupsClientManager.Get();
        RegisterTugaGroupsRPCs();
#ifdef CF
        CF_RegisterKeybinds(new TugaGroupsKeybinds());
#endif
    }

    override void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        HandleOpenMapInput();
        HandleAcceptInviteInput();
        TickInviteExpiry();
        UpdateGroupHud();
        UpdateNameplates();
        UpdateCompassHud();
    }

    void RegisterTugaGroupsRPCs()
    {
        GetRPCManager().AddRPC("TugaGroups", "SyncSettings", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("TugaGroups", "SyncMarkers", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("TugaGroups", "SyncGroupState", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("TugaGroups", "InviteReceived", this, SingleplayerExecutionType.Client);
    }

    void HandleOpenMapInput()
    {
        if (!TugaGroupsInputState.TriggerOpenMap)
        {
            return;
        }

        TugaGroupsInputState.TriggerOpenMap = false;

        UIScriptedMenu currentMenu = GetGame().GetUIManager().GetMenu();
        if (currentMenu && currentMenu.IsInherited(MapMenu))
        {
            return;
        }

        GetGame().GetUIManager().EnterScriptedMenu(MENU_MAP, null);
    }

    void SyncSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
        {
            return;
        }

        Param1<TugaGroupsSettings> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsClientManager.Get().State.Settings = data.param1;
    }

    void SyncMarkers(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
        {
            return;
        }

        Param4<array<ref TugaGroupsAdminMarker>, array<ref TugaGroupsGroupMarker>, array<ref TugaGroupsPingMarker>, array<ref TugaGroupsPrivateMarker>> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        state.AdminMarkers = data.param1;
        state.GroupMarkers = data.param2;
        state.Pings = data.param3;
        state.PrivateMarkers = data.param4;
        UpdateLastPing(state);
    }

    void SyncGroupState(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
        {
            return;
        }

        Param1<TugaGroupsGroup> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsClientManager.Get().State.Group = data.param1;
    }

    void InviteReceived(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type != CallType.Client)
        {
            return;
        }

        Param1<string> data;
        if (!ctx.Read(data))
        {
            return;
        }

        TugaGroupsClientManager.Get().State.PendingInviteFromId = data.param1;
        TugaGroupsClientManager.Get().State.PendingInviteExpiresAt = GetGame().GetTime() + 30000;
    }

    void HandleAcceptInviteInput()
    {
        if (!TugaGroupsInputState.TriggerAcceptInvite)
        {
            return;
        }

        TugaGroupsInputState.TriggerAcceptInvite = false;

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (state.PendingInviteFromId != string.Empty && GetGame().GetTime() < state.PendingInviteExpiresAt)
        {
            TugaGroupsClientManager.Get().SendAccept();
            state.PendingInviteFromId = string.Empty;
        }
    }

    void TickInviteExpiry()
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (state.PendingInviteFromId != string.Empty && GetGame().GetTime() > state.PendingInviteExpiresAt)
        {
            state.PendingInviteFromId = string.Empty;
        }
    }

    void UpdateGroupHud()
    {
        TugaGroupsClientManager manager = TugaGroupsClientManager.Get();
        if (!manager || !manager.GroupHud)
        {
            return;
        }

        TugaGroupsGroup group = manager.State.Group;
        if (!group)
        {
            return;
        }

        manager.GroupHud.SetSide(manager.State.NameListSide);
        manager.GroupHud.UpdateList(group);
    }

    void UpdateNameplates()
    {
        TugaGroupsClientManager manager = TugaGroupsClientManager.Get();
        if (!manager || !manager.Nameplate)
        {
            return;
        }

        TugaGroupsGroup group = manager.State.Group;
        if (!group)
        {
            return;
        }

        manager.Nameplate.Update(group, manager.State.GroupNameColor);
    }

    void UpdateCompassHud()
    {
        TugaGroupsClientManager manager = TugaGroupsClientManager.Get();
        if (!manager || !manager.CompassHud)
        {
            return;
        }

        manager.CompassHud.Update(manager.State);
    }

    void UpdateLastPing(TugaGroupsClientState state)
    {
        state.HasPingPosition = false;
        if (!state.Pings || state.Pings.Count() == 0)
        {
            return;
        }

        TugaGroupsPingMarker latest = state.Pings[0];
        foreach (TugaGroupsPingMarker ping : state.Pings)
        {
            if (ping.CreatedAt > latest.CreatedAt)
            {
                latest = ping;
            }
        }

        state.LastPingPosition = latest.Position;
        state.HasPingPosition = true;
    }
};
