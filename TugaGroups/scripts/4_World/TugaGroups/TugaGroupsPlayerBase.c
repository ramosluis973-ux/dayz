modded class PlayerBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        if (GetGame().IsClient() && !GetGame().IsDedicatedServer())
        {
            TugaGroupsClientManager manager = TugaGroupsClientManager.Get();
            if (manager)
            {
                manager.SendAddPrivateMarker(GetPosition(), "Death", "Skull", 0xFFFF0000, false);
            }
        }
    }
};
