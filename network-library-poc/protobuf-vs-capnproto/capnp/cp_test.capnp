@0xbf811a3d36d5aeec;

struct Person {
  id @0 :UInt32;
  name @1 :Text;
  email @2 :Text;
  phones @3 :List(PhoneNumber);
  address @5: Text;
  nationality @6: Text;
  birthday @7: Text;
  creationDate @8: Text;
  updateDate @9: Text;
  selfIntroduction @10: Text;
  scores @4: List(Float32);
  struct PhoneNumber {
    number @0 :UInt32;
    type @1 :Type;

    enum Type {
      mobile @0;
      home @1;
      work @2;
    }
  }
}

struct University {
  students @0 :List(Person);
}
