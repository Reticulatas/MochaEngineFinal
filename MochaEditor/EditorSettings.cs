using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace MochaEditor {

public class EditorSettings
{
    static ObservableCollection<string> _recentmaps = new ObservableCollection<string>();
    public static ObservableCollection<string> RecentMaps { get { return _recentmaps; } set { _recentmaps = value; } }

    public static void AddRecentMap(string map)
    {
        System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
        {
            RecentMaps.Add(map);
            if (_recentmaps.Count > 5)
            {
                for (int i = 0; i < _recentmaps.Count - 5; ++i)
                    _recentmaps.RemoveAt(i);
            }
        }));
    }
}
}

