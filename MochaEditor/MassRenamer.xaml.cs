using MochaInterface;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Xna.Framework;

namespace MochaEditor
{
    /// <summary>
    /// Interaction logic for MassRenamer.xaml
    /// </summary>
    public partial class MassRenamer : UserControl
    {
        public MassRenamer()
        {
            InitializeComponent();
        }

        private void CommitButton_Click(object sender, RoutedEventArgs e)
        {
            int i = 0;
            foreach (GameObject g in EngineManagerViewModel.instance.SelectedGameObjects)
            {
                ++i;

                //do replace pass first
                if (ReplaceText.Text.Count() != 0)
                {
                    g.SetName(ReplaceText.Text);
                }

                //then do prefix and suffix
                if (AppendText.Text.Count() != 0)
                {
                    g.SetName(g.GetName() + AppendText.Text);
                }
                if (PrefixText.Text.Count() != 0)
                {
                    g.SetName(PrefixText.Text + g.GetName());
                }

                //do append numbers pass last
                if (AppendNumbers.IsChecked.HasValue && AppendNumbers.IsChecked.Value)
                {
                    g.SetName(g.GetName() + i.ToString());
                }
            }

            ReplaceText.Clear();
            AppendText.Clear();
            PrefixText.Clear();

            EngineManagerViewModel.instance.ForceRefreshGameObjects();
        }

        private void HackButton_Click(object sender, RoutedEventArgs e)
        {
            int texindex = TextureIndex.Value.Value;
            string texname = TextureRoughSettor.Text;

            EditorSubsystemManaged.getInstance().QueueAction(new Action(() => {
            foreach (GameObject g in EngineManagerViewModel.instance.SelectedGameObjects)
            {
                CMeshRenderer cm = CastingHelper.CastTo<CMeshRenderer>(g.GetComponentByName("CMeshRenderer"));
                if (cm != null)
                {
                    Material mat = cm.GetMaterial() as Material;
                    MaterialAttribNodeMap map = mat.mMaterialData.mAttributeMetaLookup;
                    ObservableCollection<MaterialNodeModel> out_nodes = new ObservableCollection<MaterialNodeModel>();
                    MaterialAttribNodeVector nodes = map.Values.ElementAt(1);
                    {
                        MatAttribNode node = nodes[texindex];

                            object castedObj = null;
                            castedObj = CastingHelper.CastTo<TextureNode>(node);
                            if (castedObj != null)
                            {
                                (castedObj as TextureNode).SetTexture(AssetManager.getInstance().GetAsset(texname));
                            }
                            else
                                MessageBox.Show("You fucked up the node, this ain't no texture node dummy");
                    }
                }
            }
            }));
        }

        private void SelectName_Click(object sender, RoutedEventArgs e)
        {
            string find = RegexName.Text;

            EngineManagerViewModel.instance.SelectedGameObjects.Clear();
            foreach (GameObject g in EngineManagerViewModel.instance.CurrentGameObjects)
            {
                if (g.GetName().Contains(find))
                    EngineManagerViewModel.instance.SelectedGameObjects.Add(g);
            }
        }

        private void ShiftPosButton_Click(object sender, RoutedEventArgs e)
        {
            Vector3 offset = new Vector3((float)PosX.Value.Value,(float)PosY.Value.Value,(float)PosZ.Value.Value);

            foreach (GameObject g in EngineManagerViewModel.instance.CurrentGameObjects)
            {
                if (g.transform.GetParent() == null)
                {
                    g.transform.Translate(offset);
                }
            }
        }

    }
}
