import 'package:smartcharge_dashboard/pages/home/home_data.dart';
import 'package:flutter/material.dart';

import '../../themes/app_theme.dart';

enum SidenavCollapsePosition {
  bottomCenter,
  topRightMiddle,
  bottomRightMiddle,
}

class XDashSidenavSize {
  const XDashSidenavSize({required this.collapsed, required this.normal});
  final double normal;
  final double collapsed;
}

// ignore: must_be_immutable
class XDashSideNav extends StatefulWidget {
  XDashSideNav(
      {Key? key,
      this.theme,
      this.collapsePosition = SidenavCollapsePosition.bottomRightMiddle,
      required this.size,
      required this.onCollapse,
      required this.selectedNavIndex,
      required this.collapsedNav,
      required this.data,
      required this.onSelected})
      : super(key: key);

  XDashSidenavSize size;
  bool collapsedNav = false;
  List<SideMenuConfig> data;
  Function(int) onSelected;
  Function() onCollapse;
  SideBarTheme? theme;
  int selectedNavIndex;
  SidenavCollapsePosition collapsePosition;

  @override
  State<XDashSideNav> createState() => _XDashSideNavState();
}

class _XDashSideNavState extends State<XDashSideNav> {
  @override
  Widget build(BuildContext context) {
    final double width = (widget.collapsedNav ? 100 : 200).toDouble();
    return AnimatedContainer(
      duration: const Duration(milliseconds: 100),
      width: width,
      child: Stack(
        children: [
          Container(
            width: width * .8,
            decoration: BoxDecoration(
                color: AppTheme.of(context).sidebar.backgroudColor, // added
                boxShadow: [
                  BoxShadow(
                    blurRadius: 10,
                    color: AppTheme.of(context)
                        .sidebar
                        .backgroudColor!
                        .withOpacity(.5),
                    spreadRadius: 1,
                  ),
                ],
                borderRadius: BorderRadius.circular(20.0),
                gradient: LinearGradient(
                    colors: [
                      AppTheme.of(context).sidebar.backgroudColor!,
                      AppTheme.of(context).primaryBackgroudColor!,
                    ],
                    tileMode: TileMode.clamp,
                    begin: const Alignment(-1, .5),
                    end: const Alignment(5.5, .5),
                    stops: const [
                      .15,
                      5,
                    ])),
            child: ListView(children: [
              SizedBox(
                height: 100,
                child: Center(
                  child: Text(
                    "Menu",
                    style: AppTheme.of(context).sidebar.appTitle,
                  ),
                ),
              ),
              for (var i = 0; i < widget.data.length; i++) ...[
                XDashSidebarTab(
                    onTap: () {
                      widget.onSelected(i);
                    },
                    selected: widget.selectedNavIndex == i,
                    data: widget.data[i])
              ],
            ]),
          ),
          _buildCollapseButton(context)
        ],
      ),
    );
  }

  Widget _buildCollapseButton(BuildContext context) {
    Alignment alignValue = const Alignment(.5, .8);
    if (widget.collapsePosition == SidenavCollapsePosition.bottomCenter) {
      alignValue = const Alignment(.5, .8);
    } else if (widget.collapsePosition ==
        SidenavCollapsePosition.bottomRightMiddle) {
      alignValue = const Alignment(.8, .8);
    } else if (widget.collapsePosition ==
        SidenavCollapsePosition.topRightMiddle) {
      alignValue = const Alignment(.8, -.7);
    }
    return Align(
      alignment: alignValue,
      child: GestureDetector(
          behavior: HitTestBehavior.opaque,
          onTap: widget.onCollapse,
          child: Padding(
            padding: const EdgeInsets.only(bottom: 15),
            child: Container(
              height: 35,
              width: 35,
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: AppTheme.of(context).primaryBackgroudColor,
                boxShadow: [
                  BoxShadow(
                    blurRadius: 10,
                    color: AppTheme.of(context)
                        .sidebar
                        .backgroudColor!
                        .withOpacity(.5),
                    spreadRadius: 1,
                  ),
                ],
              ),
              child: Icon(
                widget.collapsedNav
                    ? Icons.arrow_forward_ios
                    : Icons.arrow_back_ios,
                color: AppTheme.of(context).sidebar.backgroudColor,
              ),
            ),
          )),
    );
  }
}

class XDashSidebarTab extends StatelessWidget {
  const XDashSidebarTab({
    Key? key,
    required this.onTap,
    this.selected = false,
    required this.data,
  }) : super(key: key);

  final bool selected;
  final SideMenuConfig data;
  final Function() onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: () {
        onTap();
      },
      child: Stack(
        alignment: AlignmentDirectional.centerStart,
        children: [
          Padding(
            padding: const EdgeInsets.all(8.0),
            child: AnimatedContainer(
                height: selected ? 60 : 50,
                duration: const Duration(milliseconds: 100),
                child: LayoutBuilder(
                  builder: (context, constraints) {
                    final showTitle = (constraints.maxWidth > 120);
                    return Row(
                        mainAxisAlignment: showTitle
                            ? MainAxisAlignment.start
                            : MainAxisAlignment.center,
                        children: [
                          Padding(
                            padding: const EdgeInsets.all(8.0),
                            child: Icon(
                              data.icon,
                              size: selected ? 30 : 24,
                              color: selected
                                  ? AppTheme.of(context)
                                      .sidebar
                                      .selectedIconsColor
                                  : AppTheme.of(context).sidebar.iconsColor,
                            ),
                          ),
                          if (showTitle)
                            Padding(
                              padding: const EdgeInsets.all(8.0),
                              child: Text(
                                data.name,
                                style: AppTheme.of(context)
                                    .styles
                                    .subTitle!
                                    .copyWith(
                                      color: selected
                                          ? AppTheme.of(context)
                                              .sidebar
                                              .selectedTitlesColor
                                          : AppTheme.of(context)
                                              .sidebar
                                              .titlesColor,
                                    ),
                              ),
                            )
                        ]);
                  },
                )),
          ),
          AnimatedContainer(
              height: selected ? 40 : 0,
              width: selected ? 5 : 0,
              color: AppTheme.of(context).sidebar.iconsColor,
              duration: const Duration(milliseconds: 100))
        ],
      ),
    );
  }
}
