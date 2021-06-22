object RegExpCalcFrm: TRegExpCalcFrm
  Left = 317
  Top = 187
  Width = 710
  Height = 424
  HelpContext = 2070
  Caption = 'RegExp calculator'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnResize = FormResize
  OnShow = FormShow
  DesignSize = (
    702
    391)
  PixelsPerInch = 120
  TextHeight = 16
  object LSrc: TLabel
    Left = 20
    Top = 30
    Width = 46
    Height = 16
    Caption = 'Source:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LRegExp: TLabel
    Left = 20
    Top = 84
    Width = 52
    Height = 16
    Caption = 'RegExp:'
  end
  object LReplace: TLabel
    Left = 20
    Top = 138
    Width = 55
    Height = 16
    Caption = 'Replace:'
  end
  object CBSrc: TComboBox
    Left = 20
    Top = 47
    Width = 670
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 16
    TabOrder = 0
    OnChange = CBSrcChange
    OnEnter = CBSrcEnter
    OnExit = CBSrcExit
  end
  object CBExpr: TComboBox
    Left = 20
    Top = 102
    Width = 670
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 16
    TabOrder = 1
    OnChange = CBExprChange
    OnEnter = CBExprEnter
    OnExit = CBExprExit
  end
  object CBReplace: TComboBox
    Left = 20
    Top = 156
    Width = 670
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 16
    TabOrder = 2
    OnChange = CBReplaceChange
    OnEnter = CBReplaceEnter
    OnExit = CBReplaceExit
  end
  object RBRegExp: TRadioButton
    Left = 20
    Top = 1
    Width = 80
    Height = 21
    Caption = '&RegExp'
    TabOrder = 3
    OnClick = RBRegExpClick
  end
  object RBPrintf: TRadioButton
    Left = 172
    Top = 1
    Width = 65
    Height = 21
    Caption = '&Printf'
    TabOrder = 4
    OnClick = RBPrintfClick
  end
  object CBCalc: TCheckBox
    Left = 241
    Top = 1
    Width = 60
    Height = 21
    Caption = '&Calc'
    TabOrder = 5
    OnClick = CBCalcClick
  end
  object RBScanf: TRadioButton
    Left = 104
    Top = 4
    Width = 61
    Height = 17
    Caption = '&Scanf'
    TabOrder = 6
    OnClick = RBScanfClick
  end
end
