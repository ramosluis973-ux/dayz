class TugaGroupsClientState
{
    ref TugaGroupsSettings Settings;
    ref TugaGroupsGroup Group;
    ref array<ref TugaGroupsAdminMarker> AdminMarkers;
    ref array<ref TugaGroupsGroupMarker> GroupMarkers;
    ref array<ref TugaGroupsPingMarker> Pings;
    ref array<ref TugaGroupsPrivateMarker> PrivateMarkers;
    int ActiveDisplayCategoryIndex;
    bool AdminMarkers3D;
    bool GroupMarkers3D;
    bool PrivateMarkers3D;
    bool PingMarkers3D;
    int GroupNameColor;
    float PingSize;
    string PingIcon;
    string NameListSide;
    int PendingInviteExpiresAt;
    vector LastPingPosition;
    bool HasPingPosition;
    string PendingInviteFromId;

    void TugaGroupsClientState()
    {
        Settings = new TugaGroupsSettings();
        AdminMarkers = new array<ref TugaGroupsAdminMarker>();
        GroupMarkers = new array<ref TugaGroupsGroupMarker>();
        Pings = new array<ref TugaGroupsPingMarker>();
        PrivateMarkers = new array<ref TugaGroupsPrivateMarker>();
        ActiveDisplayCategoryIndex = 0;
        AdminMarkers3D = false;
        GroupMarkers3D = false;
        PrivateMarkers3D = false;
        PingMarkers3D = false;
        GroupNameColor = 0xFFFFFFFF;
        PingSize = 1.0;
        PingIcon = "Ping";
        NameListSide = "Left";
        PendingInviteExpiresAt = 0;
        LastPingPosition = "0 0 0";
        HasPingPosition = false;
        PendingInviteFromId = string.Empty;
    }
};
