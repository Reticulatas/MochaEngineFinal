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
using System.Windows.Shapes;
using MochaInterface;
using System.Collections.ObjectModel;
using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Presentation;
using System.ComponentModel;

namespace MochaEditor
{
    namespace MergeMap
    {
        public class Object : INotifyPropertyChanged
        {
            public string Name { get; set; }
            public bool HasParent { get; set; }
            bool _shouldMerge;
            public bool ShouldMerge { get { return  _shouldMerge; } set { _shouldMerge = value; OnPropertyChanged("ShouldMerge"); } }

            public bool Mergeable
            {
                get
                {
                    if (HasParent)
                        return false;
                    return true;
                }
            }

            #region PROPERTYCHANGED
            public event PropertyChangedEventHandler PropertyChanged;

            protected void OnPropertyChanged(string name)
            {
                PropertyChangedEventHandler handler = PropertyChanged;
                if (handler != null)
                {
                    handler(this, new PropertyChangedEventArgs(name));
                }
            }
            #endregion
        }
    }

    /// <summary>
    /// Interaction logic for MergeMapWindow.xaml
    /// </summary>
    public partial class MergeMapWindow : ModernDialog
    {
        public Containers.SuppressableObservableCollection<MergeMap.Object> objects { get; set;}

        public MergeMapWindow(string filename)
        {
            objects = new Containers.SuppressableObservableCollection<MergeMap.Object>();

            List<MergeMap.Object> temp_merge = new List<MergeMap.Object>();
            EditorSubsystemManaged.getInstance().QueueActionAndBlock(new Action(() =>
            {
                StateSerializer ss = new StateSerializer();
                StateObjDescVector objs = ss.LoadStateInformation(filename, false);

                foreach (StateObjDesc desc in objs)
                {                    
                    temp_merge.Add(new MergeMap.Object
                    {
                        Name = desc.name,
                        HasParent = desc.hasParent,
                        ShouldMerge = true
                    });
                }

            }));
            objects.AddRange(temp_merge);

            InitializeComponent();

        }

        private void MergeButton_Click(object sender, RoutedEventArgs e)
        {
            EditorSubsystemManaged.getInstance().QueueActionAndBlock(new Action(() =>
           {
               StateSerializer ss = new StateSerializer();
               BaseState cur_state = GSM.getInstance().getStateUnderActive();
               BaseState new_state = GSM.getInstance().getActiveState();
               BoolVector mergeMask = new BoolVector();
               foreach (var obj in objects)
                   mergeMask.Add(obj.ShouldMerge);

               ss.MergeStates(new_state, cur_state, mergeMask);
           }));

            this.Close();
        }

        bool _hasLoaded = false;
        private void ModernDialog_Loaded(object sender, RoutedEventArgs e)
        {
            if (_hasLoaded)
                return;
            _hasLoaded = true;
            CollectionView view = (CollectionView)CollectionViewSource.GetDefaultView(ObjectsList.ItemsSource);
            view.SortDescriptions.Add(new SortDescription("Name", ListSortDirection.Ascending));
        }

        private void UncheckButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (var obj in objects)
            {
                if (obj.Mergeable)
                    obj.ShouldMerge =   false;
            }
        }

        private void CheckButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (var obj in objects)
            {
                if (obj.Mergeable)
                    obj.ShouldMerge = true;
            }
        }
    }
}
