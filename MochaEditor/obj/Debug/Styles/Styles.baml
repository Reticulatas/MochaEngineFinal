   M S B A M L   `   `   `  ����\  XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35` \RibbonControlsLibrary, Version=3.5.41019.1, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089X TXceed.Wpf.Toolkit, Version=2.1.0.0, Culture=neutral, PublicKeyToken=3e4669d2f30244f4  �� 
TargetType   MochaWindowStyle �� Property �� Value �� BasedOn  GrowingTextStyle �� Triggers P� Property P� Value N� EnterActions �� TargetProperty		 W� To
 g� Duration N� ExitActions  GrowingButtonStyle  bool2VisibilityConverter  WindowBackgroundColor  HighlightColor  	BlueColor  ForegroundColor  WindowBackgroundColorBrush �� Color 	 HighlightColorBrush 
 BlueColorBrush  ForegroundColorBrush  WindowButtonStyle �� 
TargetType ��Name J� SnapsToDevicePixels �� Triggers �� 
TargetName  MyWindowStyle  WindowBackgroundBrush&     System.Windows.Media.TextOptions ��Name%   System.Windows.SystemParameters$   WindowNonClientFrameThickness �� Source �Name � RowDefinitions ��Name �� Orientation'   !System.Windows.Shell.WindowChrome   IsHitTestVisibleInChrome � ToolTip#   System.Windows.SystemCommands   MinimizeWindowCommand �� RelativeSource �� Mode �� AncestorType ��Name    RestoreWindowCommand! � UseLayoutRounding"   MaximizeWindowCommand#   CloseWindowCommand$ ��Name% � Title& F�Name' �� Stretch( �� 	Converter) ]� 
Conditions* �� Property+ �� Value,   CornerRadius-   GlassFrameThickness.   UseAeroCaptionButtons  MochaTextBox$  Xceed.Wpf.Toolkit.DoubleUpDown�  �Xceed.Wpf.Toolkit.Primitives.UpDownBase`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]�  �Xceed.Wpf.Toolkit.NumericUpDown`1[[System.Nullable`1[[System.Double, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]  MochaDecTextBox%  Xceed.Wpf.Toolkit.DecimalUpDown�  �Xceed.Wpf.Toolkit.Primitives.UpDownBase`1[[System.Nullable`1[[System.Decimal, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]�	  �Xceed.Wpf.Toolkit.NumericUpDown`1[[System.Nullable`1[[System.Decimal, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]], mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]]�� 5      H 9http://schemas.microsoft.com/winfx/2006/xaml/presentation       :x,http://schemas.microsoft.com/winfx/2006/xaml      5      2ec)http://schemas.xceed.com/wpf/xaml/toolkit  5      %`  &	        &	 N     7��&	 �    7��&	 a    &	 t    &	 �    &	 �    &	 �    &	 �    7  &		     7  &	
 (    7  &	 @    7  &	 X    7 	 &	 E    7  7  7  7  7  7 
 7 
 7  &	 �    7 &	 �    7 �� 5      "  �6   .�5   
   ��  � ��$ Euphemia)�6'   5      �� 5      8   "  ��6_    5   
   P� 5       �@J�IsMouseOver .  5      �� 5      .��5      �� .��5      W� $ FontSize�6.   $	 15X�6S   $
 0:0:0.1D�6[   5      5       5      �� 5      .��5      �� .��5      W� $ FontSize�6.   $	 12X�6S   $
 0:0:0.08D�6[   5      5      5      5      �� 5      8   "  ��6^    5    
   P� 5!       �@J�IsMouseOver .  5"      �� 5#      .��5$      �� .��5%      W� $9 3(UIElement.RenderTransform).(ScaleTransform.ScaleX)�6.   $		 1.1X�6~   $
 0:0:0.1D�6�   W� 5&      $9 3(UIElement.RenderTransform).(ScaleTransform.ScaleY)�6.   $		 1.1X�6~   $
 0:0:0.1D�6�   5'      5(       5*      �� 5+      .��5,      �� .��5-      W� $9 3(UIElement.RenderTransform).(ScaleTransform.ScaleX)�6.   $		 1.0X�6~   $
 0:0:0.08D�6�   W� 5.      $9 3(UIElement.RenderTransform).(ScaleTransform.ScaleY)�6.   $		 1.0X�6~   $
 0:0:0.08D�6�   5/      50      52      54      �� 57      6#   ��59      	#FF2D2D30��65   ��5:      	#FF3F3F41��6.   ��5;      	#FF007ACC��6)   ��5<      	#FFF4F4F5��6/   �� 5>      8   69   �� 5?      8   62   �� 5@      8   6-   �� 5A      8   63   �� 5C      "  ��6&   .�5D   
   ��  � ��# �  6'   �� 5E   
    � ��
 ���� 6'   �� 5F   
    � ��$ Center=�67   �� 5G   
    � ��$ Center=�65   �� 5H   
    � ��$ 1q�6$   �� 5I   
    � �� 5J      �� 5K      " ��6"   .R�5L      � #��z��6   .+�5M      �� $ contentPresenter��6+   #��z��5N      #��z��5O      # z 5P      #��z��5Q      ��. 5R      5S       5T      P� 5U       �@J�IsMouseOver . . �5V      �� 8    � ��6;   5W      P� 5X       � �� . . �5Y      �� # � 
 6;    � ��5Z      P� 5[       � {� .  . �5\      �� $ contentPresenter��6%    �@J�Opacity$ .5Q�6V   5]      5_      5a      5b      �� 5d      "  �6"   .�5e   
   ��  � ��# �  6'   �� 5f   
    � ��# �  6'   �� 5g   
    �@�
ResizeMode$ CanResizeWithGrip=�6'   �� 5h   
    �@�UseLayoutRounding . 6.   �� 5i   
    �@  TextFormattingMode$ Display=�6;   �� 5j   
    � �� 5k      �� 5l      " �6"   .R�5m      �� $ WindowBorder��6   8��  ��63   �� # Z ._�5n      � .+�5o      �� $��1q�6%   ._�5p   "   �� .u�5q   &   � $ 
LayoutRoot��6+    5r   *   �� 5s   .   $��25 �6<   �� 5t   .   $��* �6<   �� 5u   .   $��15 �6<   .+�5w   *   �� $��1��6;   $��2��6H   $��7q�6Y   �� 5x   *   $ HeaderBackground��64   $��25��6N   #���  6Z   $	��Top=�6�   $��0��6�   �� 5y   *   $ 
Horizontal=�65   $��Right=�6N   $	��Top=�6j    . $��0��6�   .+�5z   .   �� $ minimize��6�   8�� ��65   �� # Z ��5{   2   � 5|   6   $��30��6;   $��25��6F   $z�1,0,0,1,0,1W�6R   .+�5}   :   F� ����    ��[ � z ��[ 	$��8��6R   $��7��6\   $��Center=�6g   $��Center=�6�   $��2��5~   �   ��69   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5   7   5�   /   � 5�   .   $��1,0,1,0q�63   .+�5�   2   �� $ Restore��69   $ restore��6�   $y�	Collapsed=�6�   8�� ��6J   �� # Z  ��5�   6   � 5�   :   $��30��6?   $��25��6J   ! . $z�1,0,0,1,.5,.5W�6o   .+�5�   >   F� X���Ѐ�     � z     � z ��[ �    ��- р�[ ��- �    �� р�[ �� р�[  z �     z 	$��8��6y   $��8��6�   $��Center=�6�   $��Center=�6�   $��1��5�   �   ��6=   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5�   ;   5�   3   �� 5�   2   $ Maximize��69   $ maximize��6�   8�� ��6K   �� # Z" ��5�   6   � 5�   :   $��31��6?   $��25��6J   .+�5�   >   F� +����    @B �@T� @B �@T�  z �     z 	$��9��6`   $��8��6j   $��Center=�6u   $��Center=�6�   $��2��5�   �   ��6=   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5�   ;   5�   3   5�   /   �� 5�   .   $ close��6}   8�� ��65   �� # Z# ��5�   2   � 5�   6   $��30��6;   $��25��6F   $z�1,0,0,1,0,1W�6R   .+�5�   :   F� +����        � z ��j � z     �    ��j 	$��8��6\   $��7��6f   $��Center=�6q   $��Center=�6�   $	��1.5��5�   �   ��69   �� *
Foreground+ �� $ FindAncestor=�$ ButtonL�6�   5�   7   5�   /   5�   +   �� 5�   *   $$ WindowTitleTextBlock��64   $��0��6R   #��z% 6_   $
��Left=�6~   $��CharacterEllipsis=�6�   $��Center=�6�   $��8 -1 0 0q�6�   $��16(�6�   #��z��6�   � 5�   *   $��2��6/   .+�5�   .   F� $& 
ResizeGrip��63   $y�	Collapsed=�6G   $��12��6^   $��12��6i   $��1q�6u   $��Right=�6�   8�� $��1��5�   R   $
' None=�6f   �����@B ��� ���- ��� �@KL ��� ���j ��� �@T� ��� ��ا ��� Ѐ� @T� р� �ا Ѐ�[ @T� р�[ �ا Ѐ�� @T� р�� �ا �@KL ��[ ���j ��[ �@T� ��[ ��ا ��[ Ѐ�[ @KL р�[ ��j Ѐ�� @KL р�� ��j �@T� �� ��ا �� Ѐ�� @B р�� ��- 6u   5�   +   5�   '   5�   #   5�      �� 5�      8�� $��1q�6S   y�6g   �� *
IsActive+ �� *FindAncestor+" �8(  5�      5�       5�      P� 5�       �@�WindowState$ 	Maximized=�69   . �5�      �� $ Maximize��6%    � y�$ 	Collapsed=�6Q   �� 5�      $ Restore��6%    � y�$ Visible=�6P   �� 5�      $ 
LayoutRoot��6%    � ��$ 7q�6O   5�      P� 5�       �@�WindowState$ Normal=�69   . �5�      �� $ Maximize��6%    � y�$ Visible=�6Q   �� 5�      $ Restore��6%    � y�$ 	Collapsed=�6P   5�      ]� 5�      ) 5�      �� 5�   "   * �@�
ResizeMode$+ CanResizeWithGrip=�6B   �� 5�   "   * �@�WindowState$+ Normal=�6C   ./�5�      �� $ 
ResizeGrip��6%    � y�$ Visible=�6S   5�      5�      5�      �� 5�   
    �@ WindowChrome 5�        5�      $, 0��6   $- 1q�60   . .  6H   5�      5�      �� 5�      "   6!   8   .�5�   
   ��  � ��
 �����6'   �� 5�   
    � ��
 ���� 6'   �� 5�   
    �@ 	AllowSpin . 6&   �� 5�   
    �@ FormatString$ F3��6)   5�      �� 5�      "   6$   8   .�5�   
   ��  � ��
 �����6'   �� 5�   
    � ��
 ���� 6'   �� 5�   
    �@ 	AllowSpin . 6&   �� 5�   
    �@	 FormatString$ F2��6)   5�      5�      