class TugaGroupsMarkerDialog : UIScriptedMenu
{
    private vector m_Position;
    private EditBoxWidget m_NameInput;
    private XComboBoxWidget m_IconList;
    private EditBoxWidget m_ColorInput;
    private XComboBoxWidget m_ScopeList;
    private CheckBoxWidget m_Render3d;
    private ButtonWidget m_CreateButton;
    private ButtonWidget m_CancelButton;

    override Widget Init()
    {
        Widget root = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsMarkerDialog.layout");
        m_NameInput = EditBoxWidget.Cast(root.FindAnyWidget("MarkerName"));
        m_IconList = XComboBoxWidget.Cast(root.FindAnyWidget("MarkerIcon"));
        m_ColorInput = EditBoxWidget.Cast(root.FindAnyWidget("MarkerColor"));
        m_ScopeList = XComboBoxWidget.Cast(root.FindAnyWidget("MarkerScope"));
        m_Render3d = CheckBoxWidget.Cast(root.FindAnyWidget("MarkerRender3D"));
        m_CreateButton = ButtonWidget.Cast(root.FindAnyWidget("MarkerCreate"));
        m_CancelButton = ButtonWidget.Cast(root.FindAnyWidget("MarkerCancel"));

        PopulateIcons();
        PopulateScopes();
        return root;
    }

    void SetPosition(vector position)
    {
        m_Position = position;
    }

    void PopulateIcons()
    {
        m_IconList.ClearAll();
        foreach (string icon : TugaGroupsClientManager.Get().State.Settings.IconWhitelist)
        {
            m_IconList.AddItem(icon, null, 0);
        }
        if (m_IconList.GetNumItems() > 0)
        {
            m_IconList.SetCurrentItem(0);
        }
    }

    void PopulateScopes()
    {
        m_ScopeList.ClearAll();
        m_ScopeList.AddItem("Group", null, 0);
        m_ScopeList.AddItem("Private", null, 0);
        m_ScopeList.SetCurrentItem(0);
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_CreateButton)
        {
            string name = m_NameInput.GetText();
            string icon = m_IconList.GetItemText(m_IconList.GetCurrentItem());
            string colorText = m_ColorInput.GetText();
            int color = colorText.ToInt();
            if (color == 0)
            {
                color = 0xFF00FF00;
            }

            bool renderAs3d = m_Render3d && m_Render3d.IsChecked();
            string scope = m_ScopeList.GetItemText(m_ScopeList.GetCurrentItem());
            if (scope == "Private")
            {
                TugaGroupsClientManager.Get().SendAddPrivateMarker(m_Position, name, icon, color, renderAs3d);
            }
            else
            {
                TugaGroupsClientManager.Get().SendAddGroupMarker(m_Position, name, icon, color, renderAs3d);
            }
            Close();
            return true;
        }

        if (w == m_CancelButton)
        {
            Close();
            return true;
        }

        return super.OnClick(w, x, y, button);
    }
};

modded class MapMenu
{
    private MapWidget m_MapWidget;
    private Widget m_TugaGroupsButtonsRoot;
    private ButtonWidget m_PingButton;
    private ButtonWidget m_AddMarkerButton;
    private Widget m_TugaGroupsTabsRoot;
    private ButtonWidget m_TabMap;
    private ButtonWidget m_TabGroups;
    private ButtonWidget m_TabConfig;
    private Widget m_GroupsPanel;
    private Widget m_ConfigPanel;
    private TextListboxWidget m_OnlineList;
    private TextListboxWidget m_GroupList;
    private EditBoxWidget m_OnlineSearchInput;
    private EditBoxWidget m_GroupNameInput;
    private EditBoxWidget m_GroupTagInput;
    private ButtonWidget m_CreateGroupButton;
    private ButtonWidget m_InviteButton;
    private ButtonWidget m_PromoteButton;
    private ButtonWidget m_DemoteButton;
    private ButtonWidget m_KickButton;
    private ButtonWidget m_TransferButton;
    private ButtonWidget m_LeaveButton;
    private ButtonWidget m_DisbandButton;
    private EditBoxWidget m_NameColorInput;
    private EditBoxWidget m_PingSizeInput;
    private EditBoxWidget m_PingIconInput;
    private XComboBoxWidget m_NameListSide;
    private ButtonWidget m_ConfigApplyButton;
    private ref array<string> m_OnlineIds;
    private ref array<string> m_GroupMemberIds;
    private Widget m_TugaGroupsSidebarRoot;
    private TextListboxWidget m_MarkerList;
    private ref array<int> m_RowTypes;
    private ref array<int> m_RowIndexes;
    private int m_LastClickTime;
    private int m_LastClickRow;
    private int m_ActiveTab;

    override Widget Init()
    {
        Widget root = super.Init();
        m_MapWidget = MapWidget.Cast(root.FindAnyWidget("Map"));

        m_TugaGroupsButtonsRoot = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsMapButtons.layout", root);
        if (m_TugaGroupsButtonsRoot)
        {
            m_TugaGroupsButtonsRoot.SetHandler(this);
            m_PingButton = ButtonWidget.Cast(m_TugaGroupsButtonsRoot.FindAnyWidget("TugaGroupsPing"));
            m_AddMarkerButton = ButtonWidget.Cast(m_TugaGroupsButtonsRoot.FindAnyWidget("TugaGroupsAddMarker"));
        }

        m_TugaGroupsTabsRoot = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsMapTabs.layout", root);
        if (m_TugaGroupsTabsRoot)
        {
            m_TugaGroupsTabsRoot.SetHandler(this);
            m_TabMap = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsTabMap"));
            m_TabGroups = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsTabGroups"));
            m_TabConfig = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsTabConfig"));
            m_GroupsPanel = m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsGroupsPanel");
            m_ConfigPanel = m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsConfigPanel");
            m_OnlineList = TextListboxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsOnlineList"));
            m_GroupList = TextListboxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsGroupList"));
            m_GroupNameInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsGroupName"));
            m_GroupTagInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsGroupTag"));
            m_OnlineSearchInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsOnlineSearch"));
            m_CreateGroupButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsCreateGroup"));
            m_InviteButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsInvite"));
            m_PromoteButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsPromote"));
            m_DemoteButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsDemote"));
            m_KickButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsKick"));
            m_TransferButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsTransfer"));
            m_LeaveButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsLeave"));
            m_DisbandButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsDisband"));
            m_NameColorInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsNameColor"));
            m_PingSizeInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsPingSize"));
            m_PingIconInput = EditBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsPingIcon"));
            m_NameListSide = XComboBoxWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsNameListSide"));
            m_ConfigApplyButton = ButtonWidget.Cast(m_TugaGroupsTabsRoot.FindAnyWidget("TugaGroupsConfigApply"));
        }

        m_TugaGroupsSidebarRoot = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsMarkerSidebar.layout", root);
        if (m_TugaGroupsSidebarRoot)
        {
            m_TugaGroupsSidebarRoot.SetHandler(this);
            m_MarkerList = TextListboxWidget.Cast(m_TugaGroupsSidebarRoot.FindAnyWidget("TugaGroupsMarkerList"));
            if (m_MarkerList)
            {
                m_MarkerList.ClearItems();
                m_MarkerList.AddColumn("Name", 0);
                m_MarkerList.AddColumn("Mode", 0.7);
                m_MarkerList.AddColumn("Del", 0.85);
                m_MarkerList.SetHandler(this);
            }
        }

        m_RowTypes = new array<int>();
        m_RowIndexes = new array<int>();
        m_OnlineIds = new array<string>();
        m_GroupMemberIds = new array<string>();
        m_LastClickTime = 0;
        m_LastClickRow = -1;
        m_ActiveTab = 0;

        if (m_NameListSide)
        {
            m_NameListSide.ClearAll();
            m_NameListSide.AddItem("Left", null, 0);
            m_NameListSide.AddItem("Right", null, 0);
            m_NameListSide.SetCurrentItem(0);
        }

        SetActiveTab(0);

        return root;
    }

    override void OnShow()
    {
        super.OnShow();
        GetGame().GetUIManager().ShowCursor(true);
    }

    override void OnHide()
    {
        super.OnHide();
        GetGame().GetUIManager().ShowCursor(false);
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);
        HandlePingInput();
        HandleDisplayModeCycleInput();
        RenderTugaGroupsMarkers();
        BuildMarkerList();
        BuildGroupLists();
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_PingButton)
        {
            vector cursorPos = GetCursorWorldPos();
            TugaGroupsClientManager.Get().SendPing(cursorPos);
            return true;
        }

        if (w == m_AddMarkerButton)
        {
            vector cursor = GetCursorWorldPos();
            TugaGroupsMarkerDialog dialog = TugaGroupsMarkerDialog.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_USER_END, null));
            if (dialog)
            {
                dialog.SetPosition(cursor);
            }
            return true;
        }

        if (w == m_TabMap)
        {
            SetActiveTab(0);
            return true;
        }

        if (w == m_TabGroups)
        {
            SetActiveTab(1);
            return true;
        }

        if (w == m_TabConfig)
        {
            SetActiveTab(2);
            return true;
        }

        if (w == m_CreateGroupButton)
        {
            if (m_GroupNameInput && m_GroupTagInput)
            {
                TugaGroupsClientManager.Get().SendCreateGroup(m_GroupNameInput.GetText(), m_GroupTagInput.GetText());
            }
            return true;
        }

        if (w == m_InviteButton)
        {
            int row = m_OnlineList.GetSelectedRow();
            if (row >= 0 && row < m_OnlineIds.Count())
            {
                TugaGroupsClientManager.Get().SendInvite(m_OnlineIds[row]);
            }
            return true;
        }

        if (w == m_PromoteButton)
        {
            HandleGroupMemberAction("promote");
            return true;
        }

        if (w == m_DemoteButton)
        {
            HandleGroupMemberAction("demote");
            return true;
        }

        if (w == m_KickButton)
        {
            HandleGroupMemberAction("kick");
            return true;
        }

        if (w == m_TransferButton)
        {
            HandleGroupMemberAction("transfer");
            return true;
        }

        if (w == m_LeaveButton)
        {
            TugaGroupsClientManager.Get().SendLeave();
            return true;
        }

        if (w == m_DisbandButton)
        {
            TugaGroupsClientManager.Get().SendDisband();
            return true;
        }

        if (w == m_ConfigApplyButton)
        {
            ApplyConfigInputs();
            return true;
        }

        if (w == m_MarkerList)
        {
            return HandleMarkerListClick();
        }

        return super.OnClick(w, x, y, button);
    }

    vector GetCursorWorldPos()
    {
        vector cursorPos = "0 0 0";
        if (m_MapWidget)
        {
            m_MapWidget.GetMapPos(cursorPos);
        }
        return cursorPos;
    }

    override bool OnMouseButtonDblClick(Widget w, int x, int y, int button)
    {
        if (m_MapWidget && w == m_MapWidget)
        {
            vector cursor = GetCursorWorldPos();
            TugaGroupsMarkerDialog dialog = TugaGroupsMarkerDialog.Cast(GetGame().GetUIManager().EnterScriptedMenu(MENU_USER_END, null));
            if (dialog)
            {
                dialog.SetPosition(cursor);
            }
            return true;
        }

        return super.OnMouseButtonDblClick(w, x, y, button);
    }

    void HandlePingInput()
    {
        UAInput pingInput = GetUApi().GetInputByName(TugaGroupsInputIds.PING);
        if (pingInput && pingInput.LocalPress())
        {
            vector cursorPos = GetCursorWorldPos();
            TugaGroupsClientManager.Get().SendPing(cursorPos);
        }
    }

    void RenderTugaGroupsMarkers()
    {
        if (!m_MapWidget)
        {
            return;
        }

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (!state || !state.Settings)
        {
            return;
        }

        m_MapWidget.ClearUserMarks();
        RenderAdminMarkers(state.AdminMarkers, state.Settings.AdminDistance, state.Settings.GlobalDistance, state.Settings, state.AdminMarkers3D);
        RenderGroupMarkers(state.GroupMarkers, state.Settings.GroupDistance, state.Settings.GlobalDistance, state.Settings, state.GroupMarkers3D);
        RenderPrivateMarkers(state.PrivateMarkers, state.Settings.GroupDistance, state.Settings.GlobalDistance, state.Settings, state.PrivateMarkers3D);
        RenderPingMarkers(state.Pings, state.Settings.PingDistance, state.Settings.GlobalDistance, state.Settings, state.PingMarkers3D);
        RenderGroupMemberPositions(state);
    }

    void RenderAdminMarkers(array<ref TugaGroupsAdminMarker> markers, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings, TugaGroupsSettings settings, bool category3d)
    {
        if (!markers)
        {
            return;
        }
        array<ref TugaGroupsMarkerBase> baseMarkers = new array<ref TugaGroupsMarkerBase>();
        foreach (TugaGroupsAdminMarker marker : markers)
        {
            baseMarkers.Insert(marker);
        }
        RenderMarkerListInternal(baseMarkers, typeSettings, globalSettings, settings, false, category3d);
    }

    void RenderGroupMarkers(array<ref TugaGroupsGroupMarker> markers, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings, TugaGroupsSettings settings, bool category3d)
    {
        if (!markers)
        {
            return;
        }
        array<ref TugaGroupsMarkerBase> baseMarkers = new array<ref TugaGroupsMarkerBase>();
        foreach (TugaGroupsGroupMarker marker : markers)
        {
            baseMarkers.Insert(marker);
        }
        RenderMarkerListInternal(baseMarkers, typeSettings, globalSettings, settings, false, category3d);
    }

    void RenderPingMarkers(array<ref TugaGroupsPingMarker> markers, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings, TugaGroupsSettings settings, bool category3d)
    {
        if (!markers)
        {
            return;
        }
        array<ref TugaGroupsMarkerBase> baseMarkers = new array<ref TugaGroupsMarkerBase>();
        foreach (TugaGroupsPingMarker marker : markers)
        {
            baseMarkers.Insert(marker);
        }
        RenderMarkerListInternal(baseMarkers, typeSettings, globalSettings, settings, true, category3d);
    }

    void RenderPrivateMarkers(array<ref TugaGroupsPrivateMarker> markers, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings, TugaGroupsSettings settings, bool category3d)
    {
        if (!markers)
        {
            return;
        }
        array<ref TugaGroupsMarkerBase> baseMarkers = new array<ref TugaGroupsMarkerBase>();
        foreach (TugaGroupsPrivateMarker marker : markers)
        {
            baseMarkers.Insert(marker);
        }
        RenderMarkerListInternal(baseMarkers, typeSettings, globalSettings, settings, false, category3d);
    }

    void RenderMarkerListInternal(array<ref TugaGroupsMarkerBase> markers, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings, TugaGroupsSettings settings, bool isPingList, bool category3d)
    {
        if (!markers)
        {
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        vector playerPos = "0 0 0";
        if (player)
        {
            playerPos = player.GetPosition();
        }

        float now = GetGame().GetTime();

        foreach (TugaGroupsMarkerBase marker : markers)
        {
            if (!marker)
            {
                continue;
            }

            if (isPingList && (now - marker.CreatedAt) > (settings.PingTtlSeconds * 1000))
            {
                continue;
            }

            if (marker.RenderAs3D != category3d)
            {
                continue;
            }

            float distance = vector.Distance(playerPos, marker.Position);
            if (typeSettings.MaxRenderMeters > 0 && distance > typeSettings.MaxRenderMeters)
            {
                continue;
            }

            if (globalSettings.MaxRenderMeters > 0 && distance > globalSettings.MaxRenderMeters)
            {
                continue;
            }

            string label = marker.Name;
            if (marker is TugaGroupsPingMarker && marker.Size > 0)
            {
                label = string.Format("%1 (x%2)", label, Math.Round(marker.Size * 10) / 10);
            }
            if (ShouldShowDistance(distance, typeSettings, globalSettings))
            {
                label = label + " " + FormatDistance(distance, typeSettings, globalSettings);
            }

            m_MapWidget.AddUserMark(marker.Position, label, marker.Icon, marker.Color);
        }
    }

    bool ShouldShowDistance(float distance, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings)
    {
        if (!typeSettings.ShowDistance && !globalSettings.ShowDistance)
        {
            return false;
        }

        if (typeSettings.MaxLabelMeters > 0 && distance > typeSettings.MaxLabelMeters)
        {
            return false;
        }

        if (globalSettings.MaxLabelMeters > 0 && distance > globalSettings.MaxLabelMeters)
        {
            return false;
        }

        return true;
    }

    string FormatDistance(float meters, TugaGroupsDistanceSettings typeSettings, TugaGroupsDistanceSettings globalSettings)
    {
        string units = typeSettings.DistanceUnits;
        if (units == string.Empty)
        {
            units = globalSettings.DistanceUnits;
        }

        if (units == "km" || (units == "auto" && meters >= 1000))
        {
            float km = meters / 1000;
            return string.Format("(%1 km)", Math.Round(km * 100) / 100);
        }

        return string.Format("(%1 m)", Math.Floor(meters));
    }

    void RenderGroupMemberPositions(TugaGroupsClientState state)
    {
        if (!state || !state.Group || !m_MapWidget)
        {
            return;
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        foreach (TugaGroupsGroupMember member : state.Group.Members)
        {
            foreach (Man man : players)
            {
                PlayerBase player = PlayerBase.Cast(man);
                if (!player || !player.GetIdentity())
                {
                    continue;
                }

                if (player.GetIdentity().GetId() != member.Id)
                {
                    continue;
                }

                string label = string.Format("[%1] %2", state.Group.Tag, member.Name);
                m_MapWidget.AddUserMark(player.GetPosition(), label, "Player", state.GroupNameColor);
            }
        }
    }


    void BuildMarkerList()
    {
        if (!m_MarkerList)
        {
            return;
        }

        m_MarkerList.ClearItems();
        m_RowTypes.Clear();
        m_RowIndexes.Clear();

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        AddCategoryRow("Admin", state.AdminMarkers3D, TugaGroupsMarkerType.Admin);
        AddMarkerRows(state.AdminMarkers, TugaGroupsMarkerType.Admin);

        AddCategoryRow("Group", state.GroupMarkers3D, TugaGroupsMarkerType.Group);
        AddMarkerRows(state.GroupMarkers, TugaGroupsMarkerType.Group);

        AddCategoryRow("Private", state.PrivateMarkers3D, TugaGroupsMarkerType.Private);
        AddMarkerRows(state.PrivateMarkers, TugaGroupsMarkerType.Private);

        AddCategoryRow("Pings", state.PingMarkers3D, TugaGroupsMarkerType.Ping);
        AddMarkerRows(state.Pings, TugaGroupsMarkerType.Ping);
    }

    void SetActiveTab(int tab)
    {
        m_ActiveTab = tab;
        bool showGroups = (tab == 1);
        bool showConfig = (tab == 2);

        if (m_GroupsPanel)
        {
            m_GroupsPanel.Show(showGroups);
        }
        if (m_ConfigPanel)
        {
            m_ConfigPanel.Show(showConfig);
        }
        if (m_TugaGroupsButtonsRoot)
        {
            m_TugaGroupsButtonsRoot.Show(tab == 0);
        }
        if (m_TugaGroupsSidebarRoot)
        {
            m_TugaGroupsSidebarRoot.Show(tab == 0);
        }
    }

    void BuildGroupLists()
    {
        if (!m_GroupList || !m_OnlineList)
        {
            return;
        }

        m_GroupList.ClearItems();
        m_OnlineList.ClearItems();
        m_GroupMemberIds.Clear();
        m_OnlineIds.Clear();

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        TugaGroupsGroup group = state.Group;
        if (group)
        {
            foreach (TugaGroupsGroupMember member : group.Members)
            {
                string rowText = string.Format("%1 (%2)", member.Name, RankToString(member.Rank));
                m_GroupList.AddItem(rowText, null, 0);
                m_GroupMemberIds.Insert(member.Id);
            }
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        string filter = string.Empty;
        if (m_OnlineSearchInput)
        {
            filter = m_OnlineSearchInput.GetText();
            filter.ToLower();
        }
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player || !player.GetIdentity())
            {
                continue;
            }

            string id = player.GetIdentity().GetId();
            if (group && group.HasMember(id))
            {
                continue;
            }

            string name = player.GetIdentity().GetName();
            string lowerName = name;
            lowerName.ToLower();
            if (filter != string.Empty && lowerName.IndexOf(filter) == -1)
            {
                continue;
            }

            m_OnlineList.AddItem(name, null, 0);
            m_OnlineIds.Insert(id);
        }
    }

    void ApplyConfigInputs()
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (m_NameColorInput)
        {
            state.GroupNameColor = m_NameColorInput.GetText().ToInt();
        }
        if (m_PingSizeInput)
        {
            state.PingSize = m_PingSizeInput.GetText().ToFloat();
        }
        if (m_PingIconInput)
        {
            state.PingIcon = m_PingIconInput.GetText();
        }
        if (m_NameListSide)
        {
            state.NameListSide = m_NameListSide.GetItemText(m_NameListSide.GetCurrentItem());
        }
    }

    string RankToString(int rank)
    {
        if (rank == TugaGroupsGroupRank.Leader)
        {
            return "Leader";
        }
        if (rank == TugaGroupsGroupRank.Officer)
        {
            return "Officer";
        }
        if (rank == TugaGroupsGroupRank.Member)
        {
            return "Member";
        }
        return "Recruit";
    }

    void HandleGroupMemberAction(string action)
    {
        int row = m_GroupList.GetSelectedRow();
        if (row < 0 || row >= m_GroupMemberIds.Count())
        {
            return;
        }

        string targetId = m_GroupMemberIds[row];
        if (action == "promote")
        {
            TugaGroupsClientManager.Get().SendPromote(targetId);
        }
        else if (action == "demote")
        {
            TugaGroupsClientManager.Get().SendDemote(targetId);
        }
        else if (action == "kick")
        {
            TugaGroupsClientManager.Get().SendKick(targetId);
        }
        else if (action == "transfer")
        {
            TugaGroupsClientManager.Get().SendTransferLeader(targetId);
        }
    }

    void ConvertPrivateToGroup(int index)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (index < 0 || index >= state.PrivateMarkers.Count())
        {
            return;
        }

        TugaGroupsPrivateMarker marker = state.PrivateMarkers[index];
        TugaGroupsClientManager.Get().SendAddGroupMarker(marker.Position, marker.Name, marker.Icon, marker.Color, marker.RenderAs3D);
        TugaGroupsClientManager.Get().SendRemovePrivateMarker(index);
    }

    void AddCategoryRow(string label, bool is3d, int type)
    {
        int row = m_MarkerList.AddItem(label, null, 0);
        m_MarkerList.SetItem(row, is3d ? "3D" : "2D", null, 1);
        m_MarkerList.SetItem(row, "", null, 2);
        m_RowTypes.Insert(type);
        m_RowIndexes.Insert(-1);
    }

    void AddMarkerRows(array<ref TugaGroupsMarkerBase> markers, int type)
    {
        if (!markers)
        {
            return;
        }

        for (int i = 0; i < markers.Count(); i++)
        {
            TugaGroupsMarkerBase marker = markers[i];
            int row = m_MarkerList.AddItem(marker.Name, null, 0);
            m_MarkerList.SetItem(row, marker.RenderAs3D ? "3D" : "2D", null, 1);
            m_MarkerList.SetItem(row, "X", null, 2);
            m_RowTypes.Insert(type);
            m_RowIndexes.Insert(i);
        }
    }

    bool HandleMarkerListClick()
    {
        if (!m_MarkerList)
        {
            return false;
        }

        int row = m_MarkerList.GetSelectedRow();
        int column = m_MarkerList.GetSelectedColumn();
        if (row < 0 || row >= m_RowTypes.Count())
        {
            return false;
        }

        int type = m_RowTypes[row];
        int index = m_RowIndexes[row];
        int now = GetGame().GetTime();

        if (index == -1 && column == 1)
        {
            ToggleCategoryDisplay(type);
            return true;
        }

        if (column == 2 && index >= 0)
        {
            if (type == TugaGroupsMarkerType.Private)
            {
                TugaGroupsClientManager.Get().SendRemovePrivateMarker(index);
                return true;
            }

            if (type == TugaGroupsMarkerType.Group)
            {
                TugaGroupsClientManager.Get().SendRemoveGroupMarker(index);
                return true;
            }
        }

        if (index >= 0 && column == 1)
        {
            ToggleMarkerDisplay(type, index);
            return true;
        }

        if (index >= 0)
        {
            if (m_LastClickRow == row && (now - m_LastClickTime) < 250)
            {
                if (type == TugaGroupsMarkerType.Private)
                {
                    ConvertPrivateToGroup(index);
                }
                else
                {
                    HandleDeleteRow(type, index);
                }
                return true;
            }
            m_LastClickRow = row;
            m_LastClickTime = now;
        }

        return false;
    }

    void HandleDeleteRow(int type, int index)
    {
        if (type == TugaGroupsMarkerType.Private)
        {
            TugaGroupsClientManager.Get().SendRemovePrivateMarker(index);
            return;
        }

        if (type == TugaGroupsMarkerType.Group)
        {
            TugaGroupsClientManager.Get().SendRemoveGroupMarker(index);
        }
    }

    void ToggleCategoryDisplay(int type)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (type == TugaGroupsMarkerType.Admin)
        {
            state.AdminMarkers3D = !state.AdminMarkers3D;
        }
        else if (type == TugaGroupsMarkerType.Group)
        {
            state.GroupMarkers3D = !state.GroupMarkers3D;
        }
        else if (type == TugaGroupsMarkerType.Private)
        {
            state.PrivateMarkers3D = !state.PrivateMarkers3D;
        }
        else if (type == TugaGroupsMarkerType.Ping)
        {
            state.PingMarkers3D = !state.PingMarkers3D;
        }
        ApplyCategoryRenderMode(type);
    }

    void ToggleMarkerDisplay(int type, int index)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (type == TugaGroupsMarkerType.Group && index >= 0 && index < state.GroupMarkers.Count())
        {
            state.GroupMarkers[index].RenderAs3D = !state.GroupMarkers[index].RenderAs3D;
        }
        else if (type == TugaGroupsMarkerType.Private && index >= 0 && index < state.PrivateMarkers.Count())
        {
            state.PrivateMarkers[index].RenderAs3D = !state.PrivateMarkers[index].RenderAs3D;
        }
        else if (type == TugaGroupsMarkerType.Admin && index >= 0 && index < state.AdminMarkers.Count())
        {
            state.AdminMarkers[index].RenderAs3D = !state.AdminMarkers[index].RenderAs3D;
        }
        else if (type == TugaGroupsMarkerType.Ping && index >= 0 && index < state.Pings.Count())
        {
            state.Pings[index].RenderAs3D = !state.Pings[index].RenderAs3D;
        }
    }

    void ApplyCategoryRenderMode(int type)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        bool target = GetCategoryDisplayMode(type);
        array<ref TugaGroupsMarkerBase> markers = GetMarkerArrayForType(type);
        if (!markers)
        {
            return;
        }

        foreach (TugaGroupsMarkerBase marker : markers)
        {
            marker.RenderAs3D = target;
        }
    }

    array<ref TugaGroupsMarkerBase> GetMarkerArrayForType(int type)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (type == TugaGroupsMarkerType.Admin)
        {
            array<ref TugaGroupsMarkerBase> markers = new array<ref TugaGroupsMarkerBase>();
            foreach (TugaGroupsAdminMarker marker : state.AdminMarkers)
            {
                markers.Insert(marker);
            }
            return markers;
        }

        if (type == TugaGroupsMarkerType.Group)
        {
            array<ref TugaGroupsMarkerBase> markers = new array<ref TugaGroupsMarkerBase>();
            foreach (TugaGroupsGroupMarker marker : state.GroupMarkers)
            {
                markers.Insert(marker);
            }
            return markers;
        }

        if (type == TugaGroupsMarkerType.Private)
        {
            array<ref TugaGroupsMarkerBase> markers = new array<ref TugaGroupsMarkerBase>();
            foreach (TugaGroupsPrivateMarker marker : state.PrivateMarkers)
            {
                markers.Insert(marker);
            }
            return markers;
        }

        if (type == TugaGroupsMarkerType.Ping)
        {
            array<ref TugaGroupsMarkerBase> markers = new array<ref TugaGroupsMarkerBase>();
            foreach (TugaGroupsPingMarker marker : state.Pings)
            {
                markers.Insert(marker);
            }
            return markers;
        }

        return null;
    }

    bool GetCategoryDisplayMode(int type)
    {
        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        if (type == TugaGroupsMarkerType.Admin)
        {
            return state.AdminMarkers3D;
        }
        if (type == TugaGroupsMarkerType.Group)
        {
            return state.GroupMarkers3D;
        }
        if (type == TugaGroupsMarkerType.Private)
        {
            return state.PrivateMarkers3D;
        }
        if (type == TugaGroupsMarkerType.Ping)
        {
            return state.PingMarkers3D;
        }
        return false;
    }

    void HandleDisplayModeCycleInput()
    {
        UAInput toggleInput = GetUApi().GetInputByName(TugaGroupsInputIds.TOGGLE_DISPLAY_MODE);
        if (!toggleInput || !toggleInput.LocalPress())
        {
            return;
        }

        TugaGroupsClientState state = TugaGroupsClientManager.Get().State;
        state.ActiveDisplayCategoryIndex = (state.ActiveDisplayCategoryIndex + 1) % 4;
        int type = CategoryIndexToType(state.ActiveDisplayCategoryIndex);
        ToggleCategoryDisplay(type);
    }

    int CategoryIndexToType(int index)
    {
        if (index == 0)
        {
            return TugaGroupsMarkerType.Admin;
        }
        if (index == 1)
        {
            return TugaGroupsMarkerType.Group;
        }
        if (index == 2)
        {
            return TugaGroupsMarkerType.Private;
        }
        return TugaGroupsMarkerType.Ping;
    }
};
