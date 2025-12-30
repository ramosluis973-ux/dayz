class TugaGroupsServerSettings
{
    int MaxGroupSize;
    bool ShowPingDistance;

    void TugaGroupsServerSettings()
    {
        MaxGroupSize = 8;
        ShowPingDistance = true;
    }

    static string SettingsPath()
    {
        return "$profile:TugaGroups/ServerSettings.json";
    }

    static TugaGroupsServerSettings Load()
    {
        TugaGroupsServerSettings settings = new TugaGroupsServerSettings();
        string path = SettingsPath();
        if (FileExist(path))
        {
            JsonFileLoader<TugaGroupsServerSettings>.JsonLoadFile(path, settings);
        }
        else
        {
            settings.Save();
        }
        return settings;
    }

    void Save()
    {
        MakeDirectory("$profile:TugaGroups");
        JsonFileLoader<TugaGroupsServerSettings>.JsonSaveFile(SettingsPath(), this);
    }
};
