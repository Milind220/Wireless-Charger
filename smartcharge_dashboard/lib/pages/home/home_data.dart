import 'package:flutter/material.dart';
import 'package:smartcharge_dashboard/widgets/settings_screen.dart';
import 'package:smartcharge_dashboard/widgets/status_screen.dart';
import 'package:smartcharge_dashboard/widgets/users_screen.dart';

class SideMenuConfig {
  SideMenuConfig({this.icon, required this.name, required this.view});
  final String name;
  final IconData? icon;
  final Widget view;
}

final sidebarData = [
  SideMenuConfig(
    name: "Realtime",
    icon: Icons.speed,
    view: const StatusScreen(),
  ),
  SideMenuConfig(
    name: "Users",
    icon: Icons.group,
    view: const UsersScreen(),
  ),
  SideMenuConfig(
    name: "Settings",
    icon: Icons.settings,
    view: const SettingsScreen(),
  )
];
