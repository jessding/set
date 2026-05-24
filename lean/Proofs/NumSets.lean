import MathLib

def Card := Fin 3 × Fin 3 × Fin 3 × Fin 3
  deriving Fintype, DecidableEq

def isSet (c1 c2 c3 : Card) : Prop :=
  c1.1 + c2.1 + c3.1 = 0 ∧
  c1.2.1 + c2.2.1 + c3.2.1 = 0 ∧
  c1.2.2.1 + c2.2.2.1 + c3.2.2.1 = 0 ∧
  c1.2.2.2 + c2.2.2.2 + c3.2.2.2 = 0

instance (c1 c2 c3 : Card) : Decidable (isSet c1 c2 c3) :=
  by unfold isSet; infer_instance

def cardLessThanBool (c1 c2 : Card) : Bool :=
  (c1.1 < c2.1) ||
  (c1.1 == c2.1 && c1.2.1 < c2.2.1) ||
  (c1.1 == c2.1 && c1.2.1 == c2.2.1 && c1.2.2.1 < c2.2.2.1) ||
  (c1.1 == c2.1 && c1.2.1 == c2.2.1 && c1.2.2.1 == c2.2.2.1 && c1.2.2.2 < c2.2.2.2)

instance : LT Card where
  lt c1 c2 := cardLessThanBool c1 c2 = true

instance (c1 c2 : Card) : Decidable (c1 < c2) :=
  by change Decidable (cardLessThanBool c1 c2 = true); infer_instance

def allSetsProduct : Finset (Card × Card × Card) :=
  (Finset.univ : Finset Card).biUnion fun c1 =>
    (Finset.univ : Finset Card).biUnion fun c2 =>
      if c1 < c2 then
        ((Finset.univ : Finset Card).filter fun c3 => c2 < c3 ∧ isSet c1 c2 c3).map
          ⟨fun c3 => (c1, c2, c3), fun _ _ h => by injection h; rename_i h2; injection h2⟩
      else ∅

set_option maxRecDepth 1000000

theorem card_sets_product : (allSetsProduct).card = 1080 := by decide
