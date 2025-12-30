class TugaGroupsInputState
{
    static bool TriggerOpenMap;
    static bool TriggerPing;
    static bool TriggerToggleDisplay;
    static bool TriggerAcceptInvite;

    static void Reset()
    {
        TriggerOpenMap = false;
        TriggerPing = false;
        TriggerToggleDisplay = false;
        TriggerAcceptInvite = false;
    }
};
