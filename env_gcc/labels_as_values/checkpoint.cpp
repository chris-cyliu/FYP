int main() {
  void* labels = {&&first_label, &&second_label, &&third_label};

  int checkpoint_id;
  // deserialize latest value from disk
  deserialize(checkpoint_id);

  if (checkpoint_id != -1)
    goto labels[checkpoint_id];

first_label:
  cout << "first label\n";
  checkpoint(/*checkpoint_id = */ 0);
second_label:
  cout << "second label\n";
  checkpoint(/*checkpoint_id = */ 1);
third_label:
  cout << "third label\n";
  checkpoint(/*checkpoint_id = */ 2);

  return 0;
}



