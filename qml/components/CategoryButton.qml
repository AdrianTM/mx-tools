import QtQuick
import QtQuick.Controls

Button {
    id: control

    SystemPalette { id: systemPalette }

    property bool selected: false
    property color accentColor: systemPalette.highlight
    property color mutedTextColor: systemPalette.text
    property color hoverColor: Qt.alpha(systemPalette.highlight, 0.13)

    height: 44
    leftPadding: 14
    rightPadding: 14
    hoverEnabled: true
    Accessible.name: text

    contentItem: Text {
        text: control.text
        color: control.selected ? control.accentColor : control.mutedTextColor
        font.pixelSize: control.font.pixelSize
        font.weight: control.selected ? Font.DemiBold : Font.Medium
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        radius: 10
        color: control.selected ? control.hoverColor : (control.hovered ? Qt.alpha(control.hoverColor, 0.7) : "transparent")
        border.width: control.activeFocus ? 2 : 0
        border.color: control.accentColor

        Rectangle {
            visible: control.selected
            width: 3
            height: 20
            radius: 2
            color: control.accentColor
            anchors.left: parent.left
            anchors.leftMargin: 4
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
