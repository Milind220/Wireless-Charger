import 'package:smartcharge_dashboard/pages/home/home_data.dart';
import 'package:smartcharge_dashboard/themes/app_theme.dart';
import 'package:smartcharge_dashboard/widgets/dashboard/dashboard.dart';
import 'package:smartcharge_dashboard/widgets/dashboard/dashboard_topbar.dart';
import 'package:smartcharge_dashboard/widgets/dashboard/dashboard_sidebar.dart';
import 'package:flutter/material.dart';

class HomePage extends StatefulWidget {
  const HomePage({Key? key}) : super(key: key);

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  int selectedNavIndex = 0;
  bool collapsedNav = false;
  final GlobalKey<XDashState> _dashKey = GlobalKey<XDashState>();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: AppTheme.of(context).primaryBackgroudColor,
      body: XDash(
        key: _dashKey,
        topbar: XDashTopbar(
          leading: IconButton(
            onPressed: () {
              _dashKey.currentState!.openSidenav();
            },
            icon: Icon(
              Icons.list_outlined,
              color: AppTheme.of(context).primaryIconColor,
            ),
          ),
          title: Text(
            "SmartCharge Dashboard",
            style: AppTheme.of(context).styles.title2,
          ),
        ),
        sidenav: XDashSideNav(
            collapsePosition: SidenavCollapsePosition.bottomCenter,
            size: const XDashSidenavSize(collapsed: 80, normal: 180),
            onCollapse: () {
              setState(() {
                collapsedNav = !collapsedNav;
              });
            },
            selectedNavIndex: selectedNavIndex,
            collapsedNav: collapsedNav,
            data: sidebarData,
            onSelected: (i) {
              setState(() {
                selectedNavIndex = i;
              });
            }),
        body: sidebarData[selectedNavIndex].view,
      ),
    );
  }
}
