class TugaGroupsGroupHud
{
    private Widget m_Root;
    private Widget m_ListRoot;
    private ref array<Widget> m_Rows;

    void TugaGroupsGroupHud()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsGroupHud.layout");
        if (!m_Root)
        {
            Print("[TugaGroupsGroupHud] Failed to create layout TugaGroupsGroupHud.layout");
            return;
        }
        m_ListRoot = m_Root.FindAnyWidget("TugaGroupsGroupHudListRoot");
        m_Rows = new array<Widget>();
    }

    void SetSide(string side)
    {
        if (!m_Root)
        {
            return;
        }

        if (side == "Right")
        {
            m_Root.SetPos(0.73, 0.2, true);
        }
        else
        {
            m_Root.SetPos(0.02, 0.2, true);
        }
    }

    void UpdateList(TugaGroupsGroup group)
    {
        if (!m_ListRoot || !group)
        {
            return;
        }

        ClearRows();
        PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
        vector localPos = "0 0 0";
        if (localPlayer)
        {
            localPos = localPlayer.GetPosition();
        }

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        foreach (TugaGroupsGroupMember member : group.Members)
        {
            PlayerBase target = FindPlayerById(players, member.Id);
            if (!target)
            {
                continue;
            }

            float distance = vector.Distance(localPos, target.GetPosition());
            string name = member.Name;
            float health = target.GetHealth("", "");

            // Normalize health for ProgressBar (accept both 0..1 and 0..100 ranges)
            float hpNorm = health;
            if (hpNorm > 1.0)
            {
                hpNorm = hpNorm / 100.0;
            }
            hpNorm = Math.Clamp(hpNorm, 0.0, 1.0);

            string distanceLabel = Math.Floor(distance).ToString() + "m";
            Widget rowWidget = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsGroupHudRow.layout", m_ListRoot);
            if (!rowWidget)
            {
                Print("[TugaGroupsGroupHud] Failed to create row widget");
                continue;
            }

            TextWidget rowName = TextWidget.Cast(rowWidget.FindAnyWidget("RowName"));
            TextWidget rowDist = TextWidget.Cast(rowWidget.FindAnyWidget("RowDist"));
            ProgressBarWidget rowHealth = ProgressBarWidget.Cast(rowWidget.FindAnyWidget("RowHealth"));
            if (rowName)
            {
                rowName.SetText(name);
            }
            if (rowDist)
            {
                rowDist.SetText(distanceLabel);
            }
            if (rowHealth)
            {
                rowHealth.SetCurrent(hpNorm);
                int color = hpNorm > 0.0 ? ARGB(255, 0, 200, 0) : ARGB(255, 200, 0, 0);
                rowHealth.SetColor(color);
            }
            m_Rows.Insert(rowWidget);
        }
    }

    void ClearRows()
    {
        foreach (Widget row : m_Rows)
        {
            if (row)
            {
                row.Unlink();
            }
        }
        m_Rows.Clear();
    }

    PlayerBase FindPlayerById(array<Man> players, string playerId)
    {
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
};
