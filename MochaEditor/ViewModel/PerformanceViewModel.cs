using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;
using System.ComponentModel;
using MochaInterface;

namespace MochaEditor.ViewModel
{
        public struct PerfData {
            public float AvgRunTime { get; set; }
            public string SystemName { get; set; }
        }
    public class PerformanceViewModel : INotifyPropertyChanged
    {
        uint _refreshInterval;
        public uint RefreshInterval { get { return _refreshInterval; } set { _refreshInterval = value; } }
        public event EventHandler OnRefresh;

        public ObservableCollection<PerfData> PerfTimeData { get; set; }
        public OperationList schedule;

        public String statusText { get; set; }

        public PerformanceViewModel()
        {
            PerfTimeData = new ObservableCollection<PerfData>();
            RefreshInterval = 5;
        }

        public void generate_schedule(object sender, EventArgs e)
        {
            if (Models.EngineManagerModel.instance != null && Models.EngineManagerModel.instance.IsConnected)
            {
                //get the schedule
                schedule = Barrista.getInstance().Schedule_Generate();
                if (OnRefresh != null)
                    OnRefresh(sender, e);

                //generate system -> time collection
                float totalTime = 0;
                PerfTimeData.Clear();
                OperationList.OperationListEnumerator en = schedule.GetEnumerator();
                while (en.MoveNext())
                {
                    float timesample = (float)en.Current.samples_sum / (float)en.Current.samples_num;
                    PerfData p = new PerfData(){ AvgRunTime = timesample, SystemName = en.Current.name };
                    PerfTimeData.Add(p);
                    totalTime += timesample;
                }

                //generate fps
                statusText = "";
                statusText += "Total Frametime: " + (totalTime / 1000.0f).ToString();
                statusText += "\t\tFPS: " + ((1.0f/(totalTime / 10000.0f))*100.0f).ToString();
                PropertyChanged(this, new PropertyChangedEventArgs("statusText"));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
