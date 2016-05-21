using System;
using System.Collections.Generic;
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
using System.Collections.ObjectModel;
using MochaInterface;

namespace MochaEditor
{
    public class MaterialNodeModel
    {
        public object node {get; set;}     //actual recursable node
        public string name { get; set; }     //sanitized name for display
    }

    public sealed class MaterialToNodesConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (!(value is Material))
                return null;
            if (value == null)
                return null;
            Material mat = value as Material;
            MaterialAttribNodeMap map = mat.mMaterialData.mAttributeMetaLookup;
            ObservableCollection<MaterialNodeModel> out_nodes = new ObservableCollection<MaterialNodeModel>();
            int texture_node_slot = 0;
            foreach (MaterialAttribNodeVector nodes in map.Values)
            {
                foreach (MatAttribNode node in nodes)
                {
                    MaterialNodeModel model = new MaterialNodeModel();
                    object castedObj = null;
                    castedObj = CastingHelper.CastTo<TextureNode>(node);
                    if (castedObj == null)
                    {
                        castedObj = CastingHelper.CastTo<ColorNode>(node);
                        if (castedObj == null)
                        {
                            castedObj = CastingHelper.CastTo<UVTransformNode>(node);
                            if (castedObj == null)
                            {
                                castedObj = CastingHelper.CastTo<ShaderNode>(node);
                                if (castedObj == null)
                                    throw new InvalidOperationException("Node could not find cast target");
                            }
                        }
                    }
                    else
                    {
                        //texture node
                        switch (texture_node_slot)
                        {
                            case 0:
                            model.name = "Diffuse";
                            break;
                            case 1:
                            model.name = "Normal";
                            break;
                            case 2:
                            model.name = "Specular";
                            break;
                            case 3:
                            model.name = "Roughness";
                            break;
                            case 4:
                            model.name = "Metallic";
                            break;
                        }
                        texture_node_slot++;
                    }

                    model.node = castedObj;

                    out_nodes.Add(model);
                }
            }
            return out_nodes;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    /// <summary>
    /// Interaction logic for PS_Material.xaml
    /// </summary>
    public partial class PS_Material : UserControl
    {

        public PS_Material()
        {
            InitializeComponent();
        }
    }
}
