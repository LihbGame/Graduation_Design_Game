   M S B A M L   `   `   `  ����
  Menu3D    Menu3D.OptionSelector   5      R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     6B   -   5          >�Name$  OptionSelectorControl��5      
������    TransparentWhiteBrush95#��[  5       � 	Resources 5         OptionSelectorButtonTemplate �� 
TargetType �Name J� RenderTransformOrigin A�Name �� Opacity  GlowOrangeColor30 � Color � Offset  GlowOrangeColor0	 F�Name
 �� StrokeStartLineCap �� StrokeEndLineCap �� Stretch  TransparentWhiteColor75 �� Color � 	Resources  Key �� TargetProperty �� 
TargetName9  3System.Windows.Media.Animation.EasingDoubleKeyFrame O� KeyTime O� Value8  2System.Windows.Media.Animation.EasingColorKeyFrame �� KeyTime  SolidOrangeColor �� Value �� Triggers P� Property P� Value N� EnterActions  HoverOn N� ExitActions  HoverOff ^� 
Conditions �� Binding �� RelativeSource �� Value �� ElementName 	 ActiveOn 
 	ActiveOff  OptionSelectorButtonStyle �� 
TargetType  �� Property! �� Value�� 5	   
   %Y
  &	       7  7  7  7  7  7  7  7 	 7 
 &	 �	    7  �� 5
      " ��6C   .R�5      � $
 Root��6   
������ $ 0.5,0.5&�6>   z�5      V� 5      .~�5      �� �� 5      �� 5      T� 5      5      .+�5      A� $ 	ArrowGlow��6   $��26��61   $��36��6<   $��10,-2q�6H   ��5      
� 5      $ 0Q�62   .��5   "   � 8   $ 0Q�6Z   � 5   "   8  $ 1Q�6Y   5      5      F� 5      $	 Arrow��6   "���� �     �     � � �  6n	$
 Round=�6B   $ Round=�6]   $��8��6v   $��20��6�   $��30��6�   $
 Fill=�6�   $
��10,0q�6�   ��5      �� 5      8  6.   5       5!       5"      ���� 5#      HoverOn .��5$      U� $" (Shape.Fill).(Brush.Opacity)�68   $ 	ArrowGlow��6q   .\�5%        $ 0:0:0.2��63   $ 1Q�6E   5&      U� 5'      $V P(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleX)�68   $
 Root��6�   .\�5(        $ 0:0:0.2��63   $
 1.05Q�6E   5)      U� 5*      $V P(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleY)�68   $
 Root��6�   .\�5+        $ 0:0:0.2��63   $
 1.05Q�6E   5,      5-      �� 5.      HoverOff .��5/      U� $" (Shape.Fill).(Brush.Opacity)�68   $ 	ArrowGlow��6q   .\�50        $ 0:0:0.2��63   $ 0Q�6E   51      U� 52      $V P(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleX)�68   $
 Root��6�   .\�53        $ 0:0:0.2��63   $ 1Q�6E   54      U� 55      $V P(UIElement.RenderTransform).(TransformGroup.Children)[0].(ScaleTransform.ScaleY)�68   $
 Root��6�   .\�56        $ 0:0:0.2��63   $ 1Q�6E   57      58      �� 59      ActiveOn .��5:      �� $, &(Shape.Stroke).(SolidColorBrush.Color)�67   $ Arrow��6z   .g�5;        $ 0:0:0.2��62   8  6D   5<      5=      �� 5>      	ActiveOff .��5?      �� $, &(Shape.Stroke).(SolidColorBrush.Color)�67   $ Arrow��6z   .g�5@        $ 0:0:0.2��62   8  6D   5A      5B      5C       5D      P� 5E       �@J�IsMouseOver .  5F      �� 5G      8�� 6.    5I      �� 5J      8�� 6.   5L      ^� 5M       5N      �� 5O       6(   �� *	IsEnabled+ �� *Self+ True6l   �� 5P       6(   �� *IsKeyboardFocused+$ OptionSelectorControl�� True6q    5R      �� 5S      8�� 6.    5U      �� 5V      8�� 6.   5X      5Z      �� 5[      " ��66   .�5\      ��   � |�! .  6.   �� 5]        � ��8!   6-   5^      5_      .��5b      � -   5        $ 
LayoutRoot��5b      " � ColumnDefinitions" 5c   
   �� 5d      $
��Auto �6   �� 5e      �� 5f      $
��Auto �6   .+�5h   
   �� -   5        # ��Name$# 
PrevButton��5h      #��[ 6+   �� 5i   
   $��1��5j      $��0,-5,0,0q�5l      $��Center=�6   $��Center=�6<   ��5k      �� *SelectedOption+$ OptionSelectorControl��5l   <   �� 5m   
   -   5        $# 
NextButton��5m      $��2��6+   #��[ 6;   $ 0.5,0.5&�6n   z�5n      �� 5o      $ �� ScaleX$$ -1Q�6!   5q      5r      5s      