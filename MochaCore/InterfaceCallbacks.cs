using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

[assembly:InternalsVisibleTo("MochaScriptSystem")]

[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void NativeMochaDotNetCallback();
[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void NativeMochaDotNetCScriptCopyCallback(IntPtr from, IntPtr to);
[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void NativeMochaDotNetCScriptEventCallback(IntPtr from, string name, IntPtr data, string data_type);
[UnmanagedFunctionPointer(CallingConvention.StdCall)]
public delegate void NativeMochaDotNetCScriptCallback(IntPtr from);