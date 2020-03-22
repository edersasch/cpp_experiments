import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

Item {
    id: root

    property int operation_id: root.plus_id
    property int first_max: 99

    property alias step_max: step_val.to

    readonly property int plus_id: 1
    readonly property int minus_id: 2
    readonly property int multiply_id: 3
    readonly property int index_to_value_offset: start_with_0.checked ? 0 : 1
    readonly property int first_index: first_value.value - index_to_value_offset
    readonly property string plus_color: "lightgreen"
    readonly property string minus_color: "yellow"
    readonly property string multiply_color: "lightblue"

    readonly property alias last_index: slider.value
    readonly property alias step: step_val.value
    readonly property alias number_repeater: number_repeater

    signal change()

    onFirst_indexChanged: change()
    onLast_indexChanged: change()
    onStepChanged: change()
    onOperation_idChanged: change()

    anchors.fill: parent ? parent : undefined

    ColumnLayout {
        id: columnLayout

        anchors.fill: parent

        Item {
            id: range_setting_row

            Layout.fillWidth: true
            height: 50

            RowLayout {
                id: rowLayout
                anchors.horizontalCenter: parent.horizontalCenter

                Slider {
                    id: slider
                    stepSize: 10
                    to: 999
                    from: 9
                    value: 99
                }

                Label {
                    id: tolabel
                    text: slider.value + index_to_value_offset
                }
            }
        }

        Item {
            id: value_setting_row

            Layout.fillWidth: true
            height: 50

            RowLayout {
                id: secondRowLayout
                anchors.horizontalCenter: parent.horizontalCenter

                SpinBox {
                    id: first_value
                    from: 0 + index_to_value_offset
                    to: root.first_max + index_to_value_offset
                    value: 5
                    editable: true
                }

                SpinBox {
                    id: step_val
                    from: 1
                    to: slider.to
                    value: 9
                    editable: true
                }
            }
        }

        Item {
            id: operation_setting_row

            Layout.fillWidth: true
            height: 50

            ButtonGroup {
                id: op_group
            }

            RowLayout {
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 0

                RadioButton {
                    id: op_plus
                    Layout.fillHeight: true
                    checked: true
                    text: " + "
                    ButtonGroup.group: op_group

                    background: Rectangle {
                        color: root.plus_color
                        border.color: "black"
                        border.width: op_plus.checked ? 2 : 0
                    }

                    indicator: Item {}

                    onClicked: root.operation_id = root.plus_id
                }

                RadioButton {
                    id: op_minus
                    Layout.fillHeight: true
                    text: " - "
                    ButtonGroup.group: op_group

                    background: Rectangle {
                        color: root.minus_color
                        border.color: "black"
                        border.width: op_minus.checked ? 2 : 0
                    }

                    indicator: Item {}

                    onClicked: root.operation_id = root. minus_id
                }

                RadioButton {
                    id: op_multiply
                    Layout.fillHeight: true
                    text: " * "
                    ButtonGroup.group: op_group

                    background: Rectangle {
                        color: root.multiply_color
                        border.color: "black"
                        border.width: op_multiply.checked ? 2 : 0
                    }

                    indicator: Item {}

                    onClicked: root.operation_id = root. multiply_id
                }

                CheckBox {
                    id: start_with_0
                    text: qsTr("Start with 0")
                }
            }
        }

        Item {
            id: number_view

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.margins: 10

            Flickable {
                id: flickable
                anchors.fill: parent
                contentHeight: number_grid.height
                clip: true
                ScrollBar.vertical: ScrollBar {}

                GridLayout {
                    id: number_grid
                    anchors.horizontalCenter: parent.horizontalCenter
                    columns: 10

                    Repeater {
                        id: number_repeater
                        model: slider.to + 1

                        Label {
                            text: index + index_to_value_offset
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                            Layout.margins: 6
                            visible: index <= slider.value
                            background: Rectangle {
                                anchors.centerIn: parent
                                width: parent.width + 12
                                height: parent.height + 12
                            }
                        }
                    }
                }
            }
        }
    }
}
