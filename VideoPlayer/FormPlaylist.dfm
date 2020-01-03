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
    Height = 345
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
    OnKeyDown = lvPlaylistKeyDown
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
