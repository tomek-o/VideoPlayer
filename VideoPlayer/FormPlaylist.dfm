object frmPlaylist: TfrmPlaylist
  Left = 0
  Top = 0
  Align = alClient
  BorderStyle = bsNone
  ClientHeight = 345
  ClientWidth = 480
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object lvPlaylist: TListView
    Left = 0
    Top = 0
    Width = 480
    Height = 317
    Align = alClient
    Columns = <
      item
        Caption = 'File'
        Width = 300
      end
      item
        Caption = 'Size'
        Width = 80
      end>
    HideSelection = False
    MultiSelect = True
    OwnerData = True
    ReadOnly = True
    RowSelect = True
    PopupMenu = popupMenu
    TabOrder = 0
    ViewStyle = vsReport
    OnData = lvPlaylistData
    OnDblClick = lvPlaylistDblClick
    OnKeyDown = lvPlaylistKeyDown
    ExplicitHeight = 297
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 317
    Width = 480
    Height = 28
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object lblFilter: TLabel
      Left = 8
      Top = 6
      Width = 24
      Height = 13
      Caption = 'Filter'
    end
    object edFilter: TEdit
      Left = 38
      Top = 3
      Width = 155
      Height = 21
      TabOrder = 0
      OnChange = edFilterChange
    end
    object btnFilterClear: TButton
      Left = 196
      Top = 3
      Width = 38
      Height = 21
      Caption = 'Clear'
      TabOrder = 1
      OnClick = btnFilterClearClick
    end
  end
  object popupMenu: TPopupMenu
    Left = 8
    Top = 24
    object miAddFiles: TMenuItem
      Caption = 'Add file(s)'
      OnClick = miAddFilesClick
    end
  end
  object OpenDialog: TOpenDialog
    Options = [ofHideReadOnly, ofAllowMultiSelect, ofFileMustExist, ofEnableSizing]
    Left = 40
    Top = 24
  end
end
