using System;
using System.IO;
using System.Windows.Controls;
using System.Windows.Media.Imaging;
using System.Windows;
using MochaInterface;

namespace MochaEditor
{
    /// <summary>
    /// Visitor to load in assets when activated
    /// </summary>
    public class AssetLoader
    {
        AssetItem _item;

        public AssetLoader(AssetItem i) { _item = i; }

        public void LoadLevels()
        {
            EngineManagerViewModel.instance.SelectedGameObjects.Clear();
            EditorSubsystemManaged.getInstance().QueueAction(() =>
            {
                GameObjectVector v = new GameObjectVector();
                EditorSubsystem.getInstance().SetSelectedObjects(v);
                Map.LoadMap(_item.filepath);
                EditorSettings.AddRecentMap(_item.filepath);
            });
            EngineManagerViewModel.instance.currentLoadedMap = _item.filepath;
            // EngineManagerViewModel.instance.BusyMessage = null;
        }

        public void LoadPrefabs()
        {
            EditorSubsystemManaged.getInstance().QueueAction(new Action(() =>{
                GameObject g = GameObject.From(ObjectFactory.getInstance().Instantiate(_item.filename));
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    EngineManagerViewModel.instance.SelectedGameObjects.Replace(g);
                }));
            }));
        }
    }

    public class AssetImages
    {
        public BitmapImage image_sound;
        public BitmapImage image_script;
        public BitmapImage image_shader;

        AssetImages()
        {
            image_sound = new BitmapImage(new Uri(Directory.GetCurrentDirectory() + "\\Assets\\EditorAssets\\icon_audio.png"));
            image_script = new BitmapImage(new Uri(Directory.GetCurrentDirectory() + "\\Assets\\EditorAssets\\icon_script.png"));
            image_shader = new BitmapImage(new Uri(Directory.GetCurrentDirectory() + "\\Assets\\EditorAssets\\icon_shader.png"));
        }
        
        static AssetImages inst = null;
        public static AssetImages getInstance()
        {
            if (inst == null)
                inst = new AssetImages();
            return inst;
        }
    }

}