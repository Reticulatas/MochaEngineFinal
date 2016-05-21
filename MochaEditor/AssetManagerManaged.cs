using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Windows.Media.Imaging;
using System.Xml;
using System.Windows;
using MochaInterface;

namespace MochaEditor
{
    public class AssetManagerManaged
    {
        public Dictionary<string, List<string>> asset_type_to_ext = new Dictionary<string, List<string>>();
        public Dictionary<string, List<AssetItem>> asset_type_to_item = new Dictionary<string, List<AssetItem>>();
        public List<KeyValuePair<string, string>> asset_type_and_marshal_list = new List<KeyValuePair<string, string>>();
        public event EventHandler OnAssetsChanged;
        FileSystemWatcher watcher;

        private void generate_all()
        {
            asset_type_to_item.Clear();
            foreach (var asset_type in asset_type_and_marshal_list)
                generate_files(asset_type.Key, asset_type.Value);
        }

        private void generate_files(string _assetWatchedType, string _marshalType = "")
        {

            string[] files;
            try
            {
                files = Directory.GetFiles("Assets/" + _assetWatchedType + "/", "*", SearchOption.AllDirectories);
            }
            catch (System.IO.DirectoryNotFoundException)
            {
                Console.WriteLine("Assets/" + _assetWatchedType + " not found");
                return;
            }
            for (int i = 0; i < files.Length; ++i)
            {
                string ext = Path.GetExtension(files[i]).Substring(1);
                if (asset_type_to_ext[_assetWatchedType].Contains(ext))
                {
                    AssetItem item = null;
                    if (String.IsNullOrWhiteSpace(_marshalType))
                        item = new AssetItem(files[i], Path.GetFileNameWithoutExtension(files[i]), _assetWatchedType);
                    else
                    {
                        item = new AssetItem(files[i], Path.GetFileNameWithoutExtension(files[i]), _assetWatchedType);
                        item.marshal_type = _marshalType;
                    }
                    if (!String.IsNullOrWhiteSpace(item.filename))
                    {
                        if (!asset_type_to_item.ContainsKey(_assetWatchedType))
                            asset_type_to_item.Add(_assetWatchedType, new List<AssetItem>());
                        asset_type_to_item[_assetWatchedType].Add(item);
                    }
                }
            }

            if (OnAssetsChanged != null)
                OnAssetsChanged(this, null);
        }

        AssetManagerManaged()
        {
            //generate filter
            using (XmlReader reader = XmlReader.Create("Assets/Config/AssetFileSettings.xml"))
            {
                while (reader.ReadToFollowing("AssetType"))
                {
                    reader.MoveToAttribute("type");
                    string asset_type = reader.Value;

                    string asset_marshal_type = "";
                    if (reader.MoveToAttribute("marshalas"))
                      asset_marshal_type = reader.Value;
                  
                    asset_type_to_ext.Add(asset_type, new List<string>());
                    do
                    {
                        reader.Read();
                        if (reader.NodeType == XmlNodeType.Element)
                            asset_type_to_ext[asset_type].Add(reader.ReadElementContentAsString());
                    } while (reader.Name != "AssetType");

                    //generate files for this asset type
                    asset_type_and_marshal_list.Add(new KeyValuePair<string, string>(asset_type, asset_marshal_type));
                }
                generate_all();
            }
            //register reload
            watcher = new FileSystemWatcher();
            watcher.Path = "Assets";
            watcher.Filter = "*.*";
            watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.CreationTime | NotifyFilters.LastWrite;
            watcher.Changed += watcher_Changed;
            watcher.IncludeSubdirectories = true;
            watcher.EnableRaisingEvents = true;
        }

        void watcher_Changed(object sender, FileSystemEventArgs e)
        {
            if (e.ChangeType == WatcherChangeTypes.Created || e.ChangeType == WatcherChangeTypes.Deleted)
            {
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    Console.WriteLine("Assets Reloading...");
                    generate_all();
                    Console.WriteLine("...Assets Reloaded.");
                }));
            }
        }

        static AssetManagerManaged inst = null;
        public static AssetManagerManaged getInstance()
        {
            if (inst == null)
                inst = new AssetManagerManaged();
            return inst;
        }
    }

    public class AssetItem
    {
        public string filepath { get; set; }
        public string filename { get; set; }
        public string type { get; set; }
        public string marshal_type { get; set; }
        public string fileid { get; set; } //filename with ext
        public BitmapImage img { get; set; }

        public override string ToString()
        {
            return filename;
        }

        public AssetItem(string _filepath, string _filename, string _type)
        {
            filepath = _filepath;
            filename = _filename;
            fileid = Path.GetFileName(filepath);
            type = _type;
            switch (_type)
            {
                case "Sounds":
                    img = AssetImages.getInstance().image_sound;
                    break;
                case "Scripts":
                    img = AssetImages.getInstance().image_script;
                    break;
                case "Shaders":
                    img = AssetImages.getInstance().image_shader;
                    break;
            }
        }

        //occurs when item is double clicked
        public void OnPreview()
        {
            AssetLoader sl = new AssetLoader(this);
            MethodInfo m = sl.GetType().GetMethod("Load" + type);
            if (m != null)
                m.Invoke(sl, null);
            else
                Console.WriteLine("No Loader found for asset type: " + type);
        }

        public bool CanDropOn(AssetHandle handle)
        {
            if (!handle.Valid())
                return true;
            string underlyingtype = handle.GetUnderlyingAssetType();
            if (underlyingtype == type)
                return true;
            if (marshal_type != null && underlyingtype == marshal_type)
                return true;
            return false;
        }
    }
}
