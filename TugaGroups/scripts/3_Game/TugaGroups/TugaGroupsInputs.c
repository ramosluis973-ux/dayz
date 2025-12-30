class TugaGroupsInputIds
{
    static const string OPEN_MAP = "TugaGroups_OpenMap";
    static const string PING = "TugaGroups_Ping";
    static const string TOGGLE_DISPLAY_MODE = "TugaGroups_ToggleDisplayMode";
    static const string ACCEPT_INVITE = "TugaGroups_AcceptInvite";
};

modded class Inputs
{
    override void RegisterInputs()
    {
        super.RegisterInputs();
        AddInput(TugaGroupsInputIds.OPEN_MAP, "TugaGroups: Open Map", "TugaGroups", "M");
        AddInput(TugaGroupsInputIds.PING, "TugaGroups: Ping", "TugaGroups", "T");
        AddInput(TugaGroupsInputIds.TOGGLE_DISPLAY_MODE, "TugaGroups: Toggle Marker Display", "TugaGroups", "K");
        AddInput(TugaGroupsInputIds.ACCEPT_INVITE, "TugaGroups: Accept Invite", "TugaGroups", "CTRL+J");
    }
};
