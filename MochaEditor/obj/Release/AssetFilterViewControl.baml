   M S B A M L   `   `   `  ���� F  BMochaEditor, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null)clr-namespace:MochaEditorMochaEditor  (    "MochaEditor.AssetFilterViewControl   \ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     @d2http://schemas.microsoft.com/expression/blend/2008     +mochaclr-namespace:MochaEditor     -     � DataContext  ��  �� RelativeSource �� *Self+ � 	Resources  �� MergedDictionaries �� Source   DetailsPanelTemplate � IsSharedSizeScope  GridPanelTemplate � 	ItemWidth � 
ItemHeight��  �� $( "Styles/DataTemplateDictionary.xaml?�%e   &	        &	      �� .R��  . �� .R��  �� *
rowsInGrid+ �� *
rowsInGrid+.��]� .+�]� $	��Top=�.+��� $��1q�._��� -   $��ClearFilterControl��$��Clear���� -   $��FilterControl��$��*coffee filter*��$
��Left=�
�������]� $��Bottom=�.+��� -   $��ViewTypesComboBox��$
��Left=� �� SelectedIndex$ 0��@��� $��Details���� $
��Grid���� 	 � Maximum$		 200Q�
 � Minimum$
 40Q�$��1q� � Value �� *
rowsInGrid+ �� Mode$ TwoWay=��� -   $��
FilterView��$��Disabled=� %� IsVirtualizing .  %� VirtualizationMode$ 	Recycling=� %� 
ScrollUnit$
 Item=�#��[  ���� .R��  � ColumnDefinitions ��  z� SharedSizeGroup$ AssetIconColSize��.+�]� .+��� $	��Top=����� *
filename+�� ���� *img+