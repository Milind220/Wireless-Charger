import 'package:firebase_database/firebase_database.dart';
// import Foundations package to be able to use debug mode
import 'package:flutter/foundation.dart';
// import globals
import '../globals.dart';

class DatabaseService {
  FirebaseDatabase database = FirebaseDatabase.instance;
  DatabaseReference ref = FirebaseDatabase.instance.ref();

  void updatePowerState(bool state) async {
    int mode = state ? 1 : 0;
    await ref.update({
      "transmitter/mode": mode,
    });
    if (kDebugMode) {
      print('Power state updated: $mode');
    }
  }

  void getVoltage() async {
    final snapshot = await ref.child('receiver/voltage').get();
    if (snapshot.exists) {
      if (kDebugMode) {
        print(snapshot.value);
      }
      voltage = snapshot.value! as double;
    } else {
      if (kDebugMode) {
        print('No data available.');
      }
    }
  }

  void getCurrent() async {
    final snapshot = await ref.child('receiver/current').get();
    if (snapshot.exists) {
      if (kDebugMode) {
        print(snapshot.value);
      }
      current = snapshot.value! as double;
    } else {
      if (kDebugMode) {
        print('No data available.');
      }
    }
  }

  void getMode() async {
    final snapshot = await ref.child('transmitter/mode').get();
    if (snapshot.exists) {
      if (kDebugMode) {
        print(snapshot.value);
      }
      modeTrans = snapshot.value! as int;
    } else {
      if (kDebugMode) {
        print('No data available.');
      }
    }
  }
}
