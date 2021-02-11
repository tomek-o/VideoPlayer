object frmMain: TfrmMain
  Left = 0
  Top = 0
  BorderStyle = bsNone
  Caption = 'frmMain'
  ClientHeight = 262
  ClientWidth = 358
  Color = 66051
  Constraints.MinHeight = 120
  Constraints.MinWidth = 320
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  ShowHint = True
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnKeyDown = FormKeyDown
  OnMouseDown = pnlDragWindow
  OnMouseMove = pnlVideoMouseMove
  OnMouseWheel = FormMouseWheel
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlControl: TPanel
    Left = 0
    Top = 0
    Width = 358
    Height = 35
    Align = alTop
    TabOrder = 0
    OnMouseDown = pnlDragWindow
    OnMouseLeave = pnlControlMouseLeave
    ExplicitTop = 227
    object btnStop: TSpeedButton
      Left = 68
      Top = 2
      Width = 30
      Height = 30
      Hint = 'Stop'
      Glyph.Data = {
        8E000000424D8E000000000000003E0000002800000014000000140000000100
        010000000000500000000000000000000000020000000000000000000000FFFF
        FF00FFFFF000FFFFF000FFFFF000FFFFF000F000F000F000F000F000F000F000
        F000F000F000F000F000F000F000F000F000F000F000F000F000F000F000F000
        F000FFFFF000FFFFF000FFFFF000FFFFF000}
      OnClick = btnStopClick
    end
    object btnPlay: TSpeedButton
      Left = 4
      Top = 2
      Width = 31
      Height = 30
      Hint = 'Play'
      AllowAllUp = True
      GroupIndex = 1
      Glyph.Data = {
        8E000000424D8E000000000000003E0000002800000014000000140000000100
        010000000000500000000000000000000000020000000000000000000000FFFF
        FF00FFFFF000FFFFF000FFFFF000FFFFF000FBFFF000F8FFF000F87FF000F81F
        F000F807F000F803F000F803F000F807F000F81FF000F87FF000F8FFF000FBFF
        F000FFFFF000FFFFF000FFFFF000FFFFF000}
      OnClick = btnPlayClick
    end
    object btnPauseStill: TSpeedButton
      Left = 36
      Top = 2
      Width = 31
      Height = 30
      Hint = 'Pause/Still'
      AllowAllUp = True
      GroupIndex = 2
      Glyph.Data = {
        8E000000424D8E000000000000003E0000002800000014000000140000000100
        010000000000500000000000000000000000020000000000000000000000FFFF
        FF00FFFFF000FFFFF000FFFFF000FFFFF000F37FF000F31FF000F30FF000F303
        F000F300F000F3007000F3007000F300F000F303F000F30FF000F31FF000F37F
        F000FFFFF000FFFFF000FFFFF000FFFFF000}
      OnClick = btnPauseStillClick
    end
    object lblV: TLabel
      Left = 103
      Top = 2
      Width = 18
      Height = 13
      Caption = 'V: ?'
    end
    object lblA: TLabel
      Left = 103
      Top = 17
      Width = 19
      Height = 13
      Caption = 'A: ?'
    end
    object pnlControlRight: TPanel
      Left = 212
      Top = 1
      Width = 145
      Height = 33
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 0
      object btnClose: TSpeedButton
        Left = 112
        Top = 2
        Width = 30
        Height = 30
        Hint = 'Exit'
        Glyph.Data = {
          66010000424D6601000000000000760000002800000014000000140000000100
          040000000000F000000000000000000000001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00FFFFFFFFFFFF
          FFFFFFFF0000FFFFFFFFFFFFFFFFFFFF0000FF000FFFFFFFFF0000FF0000FF00
          00FFFFFFF00000FF0000FF00000FFFFF000000FF0000FF000000FFF000000FFF
          0000FFF000000F000000FFFF0000FFFF00000000000FFFFF0000FFFFF0000000
          00FFFFFF0000FFFFFF0000000FFFFFFF0000FFFFFF0000000FFFFFFF0000FFFF
          F000000000FFFFFF0000FFFF00000000000FFFFF0000FFF000000F000000FFFF
          0000FF000000FFF000000FFF0000FF00000FFFFF000000FF0000FF0000FFFFFF
          F00000FF0000FF000FFFFFFFFF0000FF0000FFFFFFFFFFFFFFFFFFFF0000FFFF
          FFFFFFFFFFFFFFFF0000}
        OnClick = btnCloseClick
      end
      object btnSettings: TSpeedButton
        Left = 14
        Top = 2
        Width = 30
        Height = 30
        Hint = 'Configuration'
        Glyph.Data = {
          C6050000424DC605000000000000360400002800000014000000140000000100
          08000000000090010000C40E0000C40E00000001000000010000000000000202
          0200030303000A0A0A000C0C0C000D0D0D000E0E0E000F0F0F00101010001717
          1700191919001E1E1E004C4C4C00515151005454540057575700585858006060
          60006969690070707000747474007F7F7F0080808000ACACAC00B2B2B200B3B3
          B300B8B8B800BDBDBD00C2C2C200E1E1E100E7E7E700F3F3F300F4F4F400F5F5
          F500F7F7F700F8F8F800F9F9F900FCFCFC00FFFFFF0000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000262626262326
          26160000000016262623262626262626261D111C25150000000015251C111D26
          262626261E0C000B19140000000014190B000C1E2626261D0C02000008050000
          000005080000020C1D262311000000000000000000000000000000001123261C
          0B00000000000000000000000000000B1C262220170700000000000000000000
          00000717202211111003000000000A12120A0000000003101111000000000000
          00091B24241B0900000000000000000000000000000F212626210F0000000000
          0000000000000000000E202626200E0000000000000000000000000000081A1F
          1F1A08000000000000001515130400000000060D0D0600000000041315162625
          180700000000000000000000000007182526261C0B0000000000000000000000
          0000000B1C262311000000000000000000000000000000001123261D0C010000
          08050000000005080000020C1D2626261E0C000B19140000000014190B000C1E
          26262626261D111C25150000000015251C111D26262626262626232626150000
          00001526262326262626}
        OnClick = btnSettingsClick
      end
      object btnFullscreen: TSpeedButton
        Left = 79
        Top = 2
        Width = 30
        Height = 30
        Hint = 'Fullscreen'
        AllowAllUp = True
        GroupIndex = 2
        Glyph.Data = {
          8E000000424D8E000000000000003E0000002800000014000000140000000100
          010000000000500000000000000000000000020000000000000000000000FFFF
          FF00FF9FF000FF0FF000FE07F000FF9FF000FF9FF000FFFFF000F801F000D801
          B00098019000080100000801000098019000D801B000F801F000FFFFF000FF9F
          F000FF9FF000FE07F000FF0FF000FF9FF000}
        OnClick = btnFullscreenClick
      end
      object btnMinimize: TSpeedButton
        Left = 46
        Top = 2
        Width = 30
        Height = 30
        Hint = 'Minimize'
        Glyph.Data = {
          8E000000424D8E000000000000003E0000002800000014000000140000000100
          010000000000500000000000000000000000020000000000000000000000FFFF
          FF00FFFFF000C0003000C0003000FFFFF000FFFFF000FFFFF000FFFFF000FFFF
          F000FFFFF000FFFFF000FFFFF000FFFFF000FFFFF000FFFFF000FFFFF000FFFF
          F000FFFFF000FFFFF000FFFFF000FFFFF000}
        OnClick = btnMinimizeClick
      end
    end
  end
  object pnlMain: TPanel
    Left = 0
    Top = 35
    Width = 358
    Height = 227
    Align = alClient
    BevelOuter = bvNone
    Color = 66051
    TabOrder = 1
    OnMouseDown = pnlDragWindow
    OnMouseMove = pnlVideoMouseMove
    ExplicitTop = 36
  end
  object tmrShowControl: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tmrShowControlTimer
    Left = 8
    Top = 40
  end
  object tmrCursorHide: TTimer
    Enabled = False
    Interval = 3000
    OnTimer = tmrCursorHideTimer
    Left = 8
    Top = 72
  end
  object tmrRefreshControl: TTimer
    Interval = 100
    OnTimer = tmrRefreshControlTimer
    Left = 8
    Top = 112
  end
  object tmrGetFilePos: TTimer
    OnTimer = tmrGetFilePosTimer
    Left = 8
    Top = 152
  end
  object tmrAntirepeat: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrAntirepeatTimer
    Left = 8
    Top = 192
  end
end
