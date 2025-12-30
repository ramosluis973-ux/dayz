class CfgPatches
{
    class TugaGroups
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class TugaGroups
    {
        dir = "TugaGroups";
        name = "TugaGroups";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};
        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"TugaGroups/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"TugaGroups/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"TugaGroups/scripts/5_Mission"};
            };
        };
    };
};
