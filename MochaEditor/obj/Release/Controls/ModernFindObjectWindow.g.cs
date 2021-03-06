﻿#pragma checksum "..\..\..\Controls\ModernFindObjectWindow.xaml" "{406ea660-64cf-4c82-b6f0-42d48172a799}" "FEBE9CFBB02A67013AAB893751A1F0C2"
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.34014
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

using FirstFloor.ModernUI.Presentation;
using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Windows.Converters;
using FirstFloor.ModernUI.Windows.Navigation;
using Microsoft.Windows.Controls;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;


namespace MochaEditor.Controls {
    
    
    /// <summary>
    /// ModernFindObjectWindow
    /// </summary>
    public partial class ModernFindObjectWindow : FirstFloor.ModernUI.Windows.Controls.ModernDialog, System.Windows.Markup.IComponentConnector {
        
        
        #line 8 "..\..\..\Controls\ModernFindObjectWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.TextBox InputText;
        
        #line default
        #line hidden
        
        
        #line 9 "..\..\..\Controls\ModernFindObjectWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.ListView FoundObjectsList;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/MochaEditor;component/controls/modernfindobjectwindow.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\..\Controls\ModernFindObjectWindow.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            
            #line 5 "..\..\..\Controls\ModernFindObjectWindow.xaml"
            ((MochaEditor.Controls.ModernFindObjectWindow)(target)).Loaded += new System.Windows.RoutedEventHandler(this.ModernDialog_Loaded);
            
            #line default
            #line hidden
            
            #line 5 "..\..\..\Controls\ModernFindObjectWindow.xaml"
            ((MochaEditor.Controls.ModernFindObjectWindow)(target)).Closing += new System.ComponentModel.CancelEventHandler(this.ModernDialog_Closing);
            
            #line default
            #line hidden
            return;
            case 2:
            this.InputText = ((System.Windows.Controls.TextBox)(target));
            
            #line 8 "..\..\..\Controls\ModernFindObjectWindow.xaml"
            this.InputText.TextChanged += new System.Windows.Controls.TextChangedEventHandler(this.InputText_TextChanged);
            
            #line default
            #line hidden
            return;
            case 3:
            this.FoundObjectsList = ((System.Windows.Controls.ListView)(target));
            
            #line 9 "..\..\..\Controls\ModernFindObjectWindow.xaml"
            this.FoundObjectsList.PreviewKeyUp += new System.Windows.Input.KeyEventHandler(this.FoundObjectsList_PreviewKeyUp);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}

