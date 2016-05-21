using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;
using System.Reflection;
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Globalization;

namespace MochaEditor
{
    public class CScriptToBoundScriptConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value == null)
                return null;
            MochaInterface.CScript cs = value as MochaInterface.CScript;
            if (cs == null)
                return null;

            if (MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetHasErrors())
                return null;
            if (!MochaScriptSystem.ScriptSubsystemManaged.getInstance().IsCScriptRegistered(cs))
                return null;
            var ret = MochaScriptSystem.ScriptSubsystemManaged.getInstance().GetBoundScriptFromCScript(cs);
            return ret;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new Exception("The method or operation is not implemented.");
        }
    }
}