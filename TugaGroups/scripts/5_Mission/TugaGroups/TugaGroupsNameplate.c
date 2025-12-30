class TugaGroupsNameplate
{
    private const float MAX_DISTANCE = 2000;

    void Update(TugaGroupsGroup group, int color)
    {
        if (!group)
        {
            return;
        }

        PlayerBase localPlayer = PlayerBase.Cast(GetGame().GetPlayer());
        if (!localPlayer)
        {
            return;
        }

        vector localPos = localPlayer.GetPosition();
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
            if (distance > MAX_DISTANCE)
            {
                continue;
            }

            string label = string.Format("[%1] %2", group.Tag, member.Name);
            target.SetNameOverride(label);
        }
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
