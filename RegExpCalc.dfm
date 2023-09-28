object RegExpCalcFrm: TRegExpCalcFrm
  Left = 317
  Top = 187
  Width = 710
  Height = 464
  HelpContext = 2070
  Caption = 'RegExp calculator'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
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
    694
    425)
  PixelsPerInch = 96
  TextHeight = 13
  object LSrc: TLabel
    Left = 16
    Top = 24
    Width = 37
    Height = 13
    Caption = 'Source:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object LRegExp: TLabel
    Left = 16
    Top = 68
    Width = 41
    Height = 13
    Caption = 'RegExp:'
  end
  object LReplace: TLabel
    Left = 16
    Top = 112
    Width = 43
    Height = 13
    Caption = 'Replace:'
  end
  object CBSrc: TComboBox
    Left = 16
    Top = 38
    Width = 545
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 0
    OnChange = CBSrcChange
    OnEnter = CBSrcEnter
    OnExit = CBSrcExit
  end
  object CBExpr: TComboBox
    Left = 16
    Top = 83
    Width = 545
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 1
    OnChange = CBExprChange
    OnEnter = CBExprEnter
    OnExit = CBExprExit
  end
  object CBReplace: TComboBox
    Left = 16
    Top = 127
    Width = 545
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 2
    OnChange = CBReplaceChange
    OnEnter = CBReplaceEnter
    OnExit = CBReplaceExit
  end
  object RBRegExp: TRadioButton
    Left = 16
    Top = 1
    Width = 65
    Height = 17
    Caption = '&RegExp'
    TabOrder = 3
    OnClick = RBRegExpClick
  end
  object RBPrintf: TRadioButton
    Left = 140
    Top = 1
    Width = 53
    Height = 17
    Caption = '&Printf'
    TabOrder = 4
    OnClick = RBPrintfClick
  end
  object CBCalc: TCheckBox
    Left = 196
    Top = 1
    Width = 49
    Height = 17
    Caption = '&Calc'
    TabOrder = 5
    OnClick = CBCalcClick
  end
  object RBScanf: TRadioButton
    Left = 85
    Top = 3
    Width = 49
    Height = 14
    Caption = '&Scanf'
    TabOrder = 6
    OnClick = RBScanfClick
  end
end
