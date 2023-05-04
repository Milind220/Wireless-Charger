import 'package:flutter/cupertino.dart';
import 'package:smartcharge_dashboard/pages/home/home_page.dart';

abstract class AppRoutes {
  static const home = "/";
  static const status = "/status";
  static const users = "/users";

  static const intial = AppRoutes.home;

  static Map<String, Widget Function(BuildContext)> getRoutes(
      BuildContext context) {
    return {
      AppRoutes.home: (context) => const HomePage(),
    };
  }
}
