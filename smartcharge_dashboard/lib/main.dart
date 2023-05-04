import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:smartcharge_dashboard/routes.dart';
import 'firebase_options.dart';

final GlobalKey<State> globalKey = GlobalKey();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  runApp(
    const MyApp(),
  );
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      key: globalKey,
      title: 'SmartCharge Dashboard',
      initialRoute: AppRoutes.intial,
      routes: AppRoutes.getRoutes(context),
    );
  }
}
