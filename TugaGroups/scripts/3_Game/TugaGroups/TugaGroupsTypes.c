enum TugaGroupsGroupRank
{
    Recruit = 0,
    Member = 1,
    Officer = 2,
    Leader = 3
};

enum TugaGroupsMarkerType
{
    Admin = 0,
    Group = 1,
    Ping = 2,
    Private = 3
};

class TugaGroupsGroupMember
{
    string Id;
    string Name;
    int Rank;

    void TugaGroupsGroupMember(string id, string name, int rank)
    {
        Id = id;
        Name = name;
        Rank = rank;
    }
};

class TugaGroupsGroup
{
    string GroupId;
    string Name;
    string Tag;
    string LeaderId;
    ref array<ref TugaGroupsGroupMember> Members;

    void TugaGroupsGroup()
    {
        Members = new array<ref TugaGroupsGroupMember>;
    }

    bool HasMember(string id)
    {
        foreach (TugaGroupsGroupMember member : Members)
        {
            if (member.Id == id)
            {
                return true;
            }
        }
        return false;
    }

    TugaGroupsGroupMember GetMember(string id)
    {
        foreach (TugaGroupsGroupMember member : Members)
        {
            if (member.Id == id)
            {
                return member;
            }
        }
        return null;
    }

    void RemoveMember(string id)
    {
        for (int i = Members.Count() - 1; i >= 0; i--)
        {
            if (Members[i].Id == id)
            {
                Members.Remove(i);
            }
        }
    }

    int GetMemberCount()
    {
        return Members.Count();
    }
};

class TugaGroupsMarkerBase
{
    vector Position;
    string Name;
    string Icon;
    int Color;
    string OwnerGroupId;
    string OwnerId;
    int CreatedAt;
    bool RenderAs3D;
    float Size;
};

class TugaGroupsAdminMarker : TugaGroupsMarkerBase
{
};

class TugaGroupsGroupMarker : TugaGroupsMarkerBase
{
};

class TugaGroupsPingMarker : TugaGroupsMarkerBase
{
};

class TugaGroupsPrivateMarker : TugaGroupsMarkerBase
{
};
