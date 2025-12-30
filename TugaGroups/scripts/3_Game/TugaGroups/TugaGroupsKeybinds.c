#ifdef CF
class TugaGroupsKeybindOpenMap : CF_Keybinding
{
    override void OnKeyPress()
    {
        TugaGroupsInputState.TriggerOpenMap = true;
    }

    override string GetKeybindName()
    {
        return "Open Map";
    }

    override int GetDefaultKey()
    {
        return KeyCode.KC_M;
    }
};

class TugaGroupsKeybindPing : CF_Keybinding
{
    override void OnKeyPress()
    {
        TugaGroupsInputState.TriggerPing = true;
    }

    override string GetKeybindName()
    {
        return "Ping";
    }

    override int GetDefaultKey()
    {
        return KeyCode.KC_T;
    }
};

class TugaGroupsKeybindToggleDisplay : CF_Keybinding
{
    override void OnKeyPress()
    {
        TugaGroupsInputState.TriggerToggleDisplay = true;
    }

    override string GetKeybindName()
    {
        return "Toggle Marker Display";
    }

    override int GetDefaultKey()
    {
        return KeyCode.KC_K;
    }
};

class TugaGroupsKeybindAcceptInvite : CF_Keybinding
{
    override void OnKeyPress()
    {
        TugaGroupsInputState.TriggerAcceptInvite = true;
    }

    override string GetKeybindName()
    {
        return "Accept Invite";
    }

    override int GetDefaultKey()
    {
        return KeyCode.KC_J;
    }

    override int GetDefaultKeyModifier()
    {
        return KeyCode.KC_LCONTROL;
    }
};

class TugaGroupsKeybinds : CF_KeybindingGroup
{
    override string GetKeybindGroupName()
    {
        return "TugaGroups";
    }

    override void RegisterKeybindings()
    {
        AddKeybind(new TugaGroupsKeybindOpenMap());
        AddKeybind(new TugaGroupsKeybindPing());
        AddKeybind(new TugaGroupsKeybindToggleDisplay());
        AddKeybind(new TugaGroupsKeybindAcceptInvite());
    }
};
#endif
