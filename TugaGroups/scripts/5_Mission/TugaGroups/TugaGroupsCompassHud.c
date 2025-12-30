class TugaGroupsCompassHud
{
    private Widget m_Root;
    private TextWidget m_Text;

    void TugaGroupsCompassHud()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("TugaGroups/gui/layouts/TugaGroupsCompassHud.layout");
        m_Text = TextWidget.Cast(m_Root.FindAnyWidget("TugaGroupsCompassText"));
    }

    void Update(TugaGroupsClientState state)
    {
        if (!m_Text)
        {
            return;
        }

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
        if (!player)
        {
            return;
        }

        float heading = GetHeading(player.GetOrientation()[0]);
        string pingText = "--";
        if (state && state.HasPingPosition)
        {
            float bearing = GetBearingTo(player.GetPosition(), state.LastPingPosition);
            pingText = string.Format("%1°", Math.Round(bearing));
        }

        m_Text.SetText(string.Format("Heading: %1° | Ping: %2", Math.Round(heading), pingText));
    }

    float GetHeading(float yaw)
    {
        float heading = yaw;
        if (heading < 0)
        {
            heading += 360;
        }
        return Math.Floor(heading);
    }

    float GetBearingTo(vector from, vector to)
    {
        vector delta = to - from;
        float angle = Math.Atan2(delta[0], delta[2]) * Math.RAD2DEG;
        if (angle < 0)
        {
            angle += 360;
        }
        return angle;
    }
};
