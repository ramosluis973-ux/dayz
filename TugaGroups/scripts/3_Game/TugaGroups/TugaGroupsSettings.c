class TugaGroupsDistanceSettings
{
    bool ShowDistance;
    string DistanceUnits;
    float MaxLabelMeters;
    float MaxRenderMeters;

    void TugaGroupsDistanceSettings()
    {
        ShowDistance = true;
        DistanceUnits = "auto";
        MaxLabelMeters = 0;
        MaxRenderMeters = 0;
    }
};

class TugaGroupsStringRules
{
    int MinLength;
    int MaxLength;
    string AllowedCharacters;
    bool AllowSpaces;

    void TugaGroupsStringRules()
    {
        MinLength = 1;
        MaxLength = 32;
        AllowedCharacters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
        AllowSpaces = true;
    }
};

class TugaGroupsActionRanks
{
    ref array<int> Invite;
    ref array<int> Accept;
    ref array<int> Leave;
    ref array<int> Kick;
    ref array<int> Promote;
    ref array<int> Demote;
    ref array<int> TransferLeader;
    ref array<int> AddGroupMarker;
    ref array<int> RemoveGroupMarker;
    ref array<int> Ping;

    void TugaGroupsActionRanks()
    {
        Invite = {TugaGroupsGroupRank.Leader, TugaGroupsGroupRank.Officer};
        Accept = {TugaGroupsGroupRank.Recruit, TugaGroupsGroupRank.Member, TugaGroupsGroupRank.Officer, TugaGroupsGroupRank.Leader};
        Leave = {TugaGroupsGroupRank.Recruit, TugaGroupsGroupRank.Member, TugaGroupsGroupRank.Officer, TugaGroupsGroupRank.Leader};
        Kick = {TugaGroupsGroupRank.Leader, TugaGroupsGroupRank.Officer};
        Promote = {TugaGroupsGroupRank.Leader};
        Demote = {TugaGroupsGroupRank.Leader};
        TransferLeader = {TugaGroupsGroupRank.Leader};
        AddGroupMarker = {TugaGroupsGroupRank.Leader, TugaGroupsGroupRank.Officer};
        RemoveGroupMarker = {TugaGroupsGroupRank.Leader, TugaGroupsGroupRank.Officer};
        Ping = {TugaGroupsGroupRank.Recruit, TugaGroupsGroupRank.Member, TugaGroupsGroupRank.Officer, TugaGroupsGroupRank.Leader};
    }
};

class TugaGroupsSettings
{
    int MaxGroupSize;
    int MaxStaticMarkersPerGroup;
    int MaxActivePingsPerGroup;
    int PingTtlSeconds;
    int InviteCooldownSeconds;
    int PingCooldownSeconds;

    ref TugaGroupsActionRanks AllowedRanks;
    ref array<string> IconWhitelist;
    ref TugaGroupsStringRules StringRules;

    ref TugaGroupsDistanceSettings GlobalDistance;
    ref TugaGroupsDistanceSettings AdminDistance;
    ref TugaGroupsDistanceSettings GroupDistance;
    ref TugaGroupsDistanceSettings PingDistance;

    void TugaGroupsSettings()
    {
        MaxGroupSize = 8;
        MaxStaticMarkersPerGroup = 25;
        MaxActivePingsPerGroup = 3;
        PingTtlSeconds = 30;
        InviteCooldownSeconds = 10;
        PingCooldownSeconds = 3;

        AllowedRanks = new TugaGroupsActionRanks();
        IconWhitelist = {"Pin", "Beacon", "Camp", "Skull"};
        StringRules = new TugaGroupsStringRules();

        GlobalDistance = new TugaGroupsDistanceSettings();
        AdminDistance = new TugaGroupsDistanceSettings();
        GroupDistance = new TugaGroupsDistanceSettings();
        PingDistance = new TugaGroupsDistanceSettings();
    }

    string SanitizeLabel(string input)
    {
        if (!input)
        {
            return "";
        }

        string trimmed = input.Trim();
        if (trimmed.Length() > StringRules.MaxLength)
        {
            trimmed = trimmed.Substring(0, StringRules.MaxLength);
        }

        string output = "";
        for (int i = 0; i < trimmed.Length(); i++)
        {
            string ch = trimmed.Substring(i, 1);
            if (ch == " " && StringRules.AllowSpaces)
            {
                output += ch;
                continue;
            }

            if (StringRules.AllowedCharacters.IndexOf(ch) != -1)
            {
                output += ch;
            }
        }

        if (output.Length() < StringRules.MinLength)
        {
            return "";
        }

        return output;
    }

    bool IsRankAllowed(int rank, array<int> allowed)
    {
        foreach (int allowedRank : allowed)
        {
            if (allowedRank == rank)
            {
                return true;
            }
        }
        return false;
    }
};
