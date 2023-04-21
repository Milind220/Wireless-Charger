import 'package:smartcharge_dashboard/pages/home/home_page.dart';
import 'package:flutter/material.dart';
import '../themes/app_theme.dart';

class SettingsScreen extends StatefulWidget {
  const SettingsScreen({Key? key}) : super(key: key);

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  @override
  Widget build(BuildContext context) {
    return Center(
        child: Container(
            color: Colors.grey,
            child: DropdownButton<int>(
              value: 0,
              hint: Text("Theme", style: AppTheme.of(context).styles.title2),
              dropdownColor: Colors.amber,
              items: ThemesModes.values
                  .map((e) => DropdownMenuItem(
                        value: e.index,
                        child: Text(
                          e.name,
                          selectionColor: Colors.white,
                        ),
                      ))
                  .toList(),
              onChanged: (int? value) {
                AppTheme.setTheme(context, ThemesModes.values[value!]);
              },
            )));
  }
}
