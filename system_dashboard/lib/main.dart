import 'package:flutter/material.dart';
import 'package:system_dashboard/constants.dart';
// add import statement for firebase core
import 'package:firebase_core/firebase_core.dart';
import 'firebase_options.dart';
// add import statement for realtime database
import 'package:firebase_database/firebase_database.dart';

FirebaseDatabase database = FirebaseDatabase.instance;
DatabaseReference ref = FirebaseDatabase.instance.ref();

void updatePowerState(bool state) async {
  await ref.update({
    "charger_1/powerState": state,
  });
  print('Power state updated');
}

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
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Flutter Demo',
      home: MyHomePage(title: 'SmartCharge Dashboard'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});
  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  bool state = true;

  void toggleState() {
    setState(() {
      state = state == false ? true : false;
    });
    updatePowerState(state);
    // ignore: avoid_print
    print(state ? 'On' : 'Off');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: kDarkBackgroundColour,
      appBar: AppBar(
        backgroundColor: kMainBackgroundColour,
        title: Text(
          widget.title,
          style: const TextStyle(color: Colors.white),
        ),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            const Text(
              'Charger Status: ',
              style: TextStyle(color: Colors.white),
            ),
            // Show on or off depending on state
            Text(
              state ? 'On' : 'Off',
              style: TextStyle(
                color: state ? Colors.green : Colors.red,
                fontSize: 30,
              ),
            ),
          ],
        ),
      ),
      floatingActionButton: FloatingActionButton(
        backgroundColor: state ? kButtonPrimaryColor : kButtonSecondaryColor,
        onPressed: toggleState,
        child: const Icon(Icons.power),
      ),
    );
  }
}
