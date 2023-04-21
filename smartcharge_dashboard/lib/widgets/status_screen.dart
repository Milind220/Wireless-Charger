import 'dart:async';
import 'package:flutter/material.dart';
import 'package:smartcharge_dashboard/firebase_funcs.dart';
import '../globals.dart';
import '../themes/app_theme.dart';

class StatusScreen extends StatefulWidget {
  const StatusScreen({Key? key}) : super(key: key);
  @override
  State<StatusScreen> createState() => _StatusScreenState();
}

String modeString = "";

class _StatusScreenState extends State<StatusScreen> {
  // upon creation of the widget, we want to start listening to the database
  @override
  void initState() {
    super.initState();
    Timer.periodic(const Duration(milliseconds: 500), (Timer t) {
      setState(() {
        DatabaseService().getVoltage();
        DatabaseService().getCurrent();
        DatabaseService().getMode();
        power = (voltage * current).toStringAsFixed(2);
        if (modeTrans == 0) {
          modeString = "OFF";
        } else if (modeTrans == 1) {
          modeString = "STANDBY";
        } else if (modeTrans == 2) {
          modeString = "ACTIVE";
        } else {
          modeString = "ERROR";
        }
        // current = getCurrent();
      });
    });
  }

  // @override
  @override
  Widget build(BuildContext context) {
    return StreamBuilder<Object>(
        stream: null,
        builder: (context, snapshot) {
          return ListView(
            children: [
              Column(
                children: [
                  Wrap(
                    alignment: WrapAlignment.start,
                    runAlignment: WrapAlignment.start,
                    children: [
                      StatsCard(
                        iconData: Icons.electric_bolt,
                        title: "$voltage V",
                        description: "Reciever\nVoltage (V)",
                      ),
                      StatsCard(
                        iconData: Icons.electric_bolt_outlined,
                        title: "$current A",
                        description: "Reciever\nCurrent (A)",
                      ),
                      StatsCard(
                        iconData: Icons.power,
                        title: "$power W",
                        description: "Output Power",
                      )
                    ],
                  ),
                  Wrap(
                    alignment: WrapAlignment.start,
                    runAlignment: WrapAlignment.start,
                    children: [
                      const StatsCard2(
                        iconData: Icons.power,
                        description: "On/Off",
                      ),
                      Padding(
                        padding: const EdgeInsets.all(8.0),
                        child: Text("Mode: $modeString",
                            style: AppTheme.of(context).styles.title2),
                      ),
                    ],
                  )
                ],
              )
            ],
          );
        });
  }
}

class StatsCard extends StatelessWidget {
  const StatsCard(
      {Key? key,
      this.description = "Description",
      this.iconData = Icons.abc,
      this.title = "Title",
      this.onMore})
      : super(key: key);
  final String title;
  final String description;
  final IconData iconData;
  final Function()? onMore;
  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      child: GestureDetector(
        child: Padding(
          padding: const EdgeInsets.all(10.0),
          child: Container(
              width: 200,
              height: 220,
              decoration: BoxDecoration(
                color: AppTheme.of(context).primaryPanelsColor, // added
                // boxShadow: [
                //   BoxShadow(
                //     blurRadius: 10,
                //     color: AppTheme.of(context)
                //         .sidebar
                //         .backgroudColor!
                //         .withOpacity(.5),
                //     spreadRadius: 1,
                //   ),
                // ],
                borderRadius: BorderRadius.circular(20.0),
              ),
              child: Stack(
                children: [
                  Padding(
                    padding: const EdgeInsets.all(20),
                    child: Column(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: [
                        Row(
                          children: [
                            Icon(
                              iconData,
                              size: 50,
                            ),
                          ],
                        ),
                        Row(
                          children: [
                            Text(
                              title,
                              style: AppTheme.of(context).styles.title3,
                            )
                          ],
                        ),
                        Row(
                          children: [
                            Flexible(
                              child: Text(
                                description,
                                style: AppTheme.of(context).styles.subTitle,
                              ),
                            )
                          ],
                        )
                      ],
                    ),
                  ),
                  Align(
                    alignment: const Alignment(.8, -.8),
                    child: IconButton(
                        onPressed: () {
                          if (onMore != null) {
                            onMore!();
                          }
                        },
                        icon: const Icon(
                          Icons.more_vert_sharp,
                          size: 30,
                        )),
                  )
                ],
              )),
        ),
      ),
    );
  }
}

class StatsCard2 extends StatefulWidget {
  const StatsCard2(
      {Key? key,
      this.description = "Description",
      this.iconData = Icons.abc,
      this.title = "Title",
      this.onMore})
      : super(key: key);
  final String title;
  final String description;
  final IconData iconData;
  final Function()? onMore;

  @override
  State<StatsCard2> createState() => _StatsCard2State();
}

class _StatsCard2State extends State<StatsCard2> {
  bool state = true;

  void toggleState() {
    setState(() {
      state = state == false ? true : false;
    });
    DatabaseService().updatePowerState(state);
  }

  @override
  Widget build(BuildContext context) {
    return MouseRegion(
      child: GestureDetector(
        // on tap, call updatePowerState

        child: Padding(
          padding: const EdgeInsets.all(10.0),
          child: Container(
              width: 300,
              height: 180,
              decoration: BoxDecoration(
                color: AppTheme.of(context).primaryPanelsColor, // added
                // boxShadow: [
                //   BoxShadow(
                //     blurRadius: 10,
                //     color: AppTheme.of(context)
                //         .sidebar
                //         .backgroudColor!
                //         .withOpacity(.5),
                //     spreadRadius: 1,
                //   ),
                // ],
                borderRadius: BorderRadius.circular(20.0),
              ),
              child: Padding(
                padding: const EdgeInsets.all(20),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  children: [
                    GestureDetector(
                      onTap: toggleState,
                      child: Center(
                        child: Icon(
                          color: state ? Colors.green : Colors.red,
                          widget.iconData,
                          size: 50,
                        ),
                      ),
                    ),
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceAround,
                      children: [
                        Flexible(
                          child: Text(
                            widget.description,
                            style: AppTheme.of(context)
                                .styles
                                .title!
                                .copyWith(fontSize: 55),
                          ),
                        ),
                      ],
                    )
                  ],
                ),
              )),
        ),
      ),
    );
  }
}
