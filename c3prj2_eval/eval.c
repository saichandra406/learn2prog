#include "eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int card_ptr_comp(const void * vp1, const void * vp2) {
	const card_t * const * cp1 = vp1;
	const card_t * const * cp2 = vp2;
	//values as well as suits in descending order
	int comp = (*cp2)->value - (*cp1)->value;
	if(comp != 0){
		return comp;
	}
	else{
		//same values different suit
	  return (*cp2)->suit - (*cp1)->suit;
	}
}

suit_t flush_suit(deck_t * hand) {
	char suit_count[4] = {0};
	for(size_t i = 0; i < hand->n_cards; i++) {
		//increment suit
		//hand select cards field
		//get the ith card pointer
		//select field suit
		suit_count[((hand->cards)[i])->suit]++;
	}
	for(int i=0; i < 4; i++){
		if(suit_count[i] >= 5)
			return (suit_t)i;
	}
  return NUM_SUITS;
}

unsigned get_largest_element(unsigned * arr, size_t n) {
  unsigned tmp;
  if(arr == NULL || n == 0)
	return 0;
	tmp = arr[0];
	for(size_t i=1; i < n; i++){
		if(arr[i] > tmp)
			tmp = arr[i];
	}
	return tmp;
}

size_t get_match_index(unsigned * match_counts, size_t n,unsigned n_of_akind){
	size_t index = -1;
	for(size_t i = 0; i < n; i++){
		if(match_counts[i] == n_of_akind){
			index = i;
			break;
		}
	}
	assert(index >=0);
  return index;
}

ssize_t  find_secondary_pair(deck_t * hand,
			     unsigned * match_counts,
			     size_t match_idx) {
		//match_counts : if its pair/2 or 3 highest same valued cards(matched cards)
		//objective: find if another pair exists other than above matched cards
		//hand is sorted. If not, passing/use of match_idx is retarded/ illogical
  size_t tail_idx = match_idx + (size_t)get_largest_element(match_counts, hand->n_cards) - 1;
		//index after the last card of the matched cards.
  size_t cur_idx = 1;
  while(cur_idx < hand->n_cards){
    if(cur_idx < match_idx || cur_idx > tail_idx){
      if(hand->cards[cur_idx - 1]->value == hand->cards[cur_idx]->value)
	return cur_idx - 1;
    }
    cur_idx++;
  }
  return -1; //if the else-block fails to find
}

int is_straight_helper(deck_t * hand, size_t index, suit_t fs, unsigned straight_count){
  unsigned nextv, count=0; //next card value, no of success comparisions
  size_t cur_idx = index;
  //need exactly 5 cards for straight, 4 for A_L straight
  //so cur_idx cannot be more than n_cards - 4
  if(cur_idx <= hand->n_cards - 4 &&
    (fs == NUM_SUITS || fs == hand->cards[cur_idx]->suit)){
    nextv = hand->cards[cur_idx]->value - 1;
    while(cur_idx < hand->n_cards - 1){
      //as we are incrementing idx after check so condition is < n - 1
      cur_idx++;
      if(hand->cards[cur_idx]->value == nextv &&
        (fs == NUM_SUITS || fs == hand->cards[cur_idx]->suit)){
	count++;
	nextv--;
	// can be return statement rather than break
	if(count == straight_count - 1)  break;
      }
    }
    //if return used inside while-loop then remove the if-statement.
    if(count == straight_count - 1) return 1;
  }
  //default else
  return 0;
  
}

int is_straight_at(deck_t * hand, size_t index, suit_t fs) {
  int ret_val; //return value
  ret_val = is_straight_helper(hand, index,fs, 5);
  
  //on ret_val = 0, check ace low straight: index start at ace
  if(ret_val == 0 && hand->cards[index]->value == VALUE_ACE
        && (fs == NUM_SUITS || fs == hand->cards[index]->suit)){
    //check straight at other cards but straight_count is 4
    //need 4 cards 5,4,3,2 to make A_L straight.
    //So index only till n_cards - 3
    while(index < hand->n_cards - 3){
      index++;
      if(hand->cards[index]->value == 5
	 &&(fs == NUM_SUITS || fs == hand->cards[index]->suit)){
	 ret_val = is_straight_helper(hand, index, fs, 4);
	 break;
      }
    }
    //this if-statement should be here, not outside	  
    if(ret_val == 1)
      return -1;
  }
  //default else
	return ret_val;
}

hand_eval_t build_hand_from_match(deck_t * hand,
				  unsigned n,
				  hand_ranking_t what,
				  size_t idx) {

  hand_eval_t ans;
  unsigned i = 0, j; // i = no. of copied cards, also idx for ans.cards
  ans.ranking = what;
  while(i < n){
		ans.cards[i] = hand->cards[idx+i];
		i++;
  }
  j = 0;
  //copy remaining card pointers if copied cards < 5
  /*
    start from j = 0
    loop through j till 5 cards are copied:	
        if j value not between [idx, idx + n):
            copy the card pointer in j index
            increment cards_copied
*/
  while(i < 5){

	  if(j < idx || j >= idx + n){
		  ans.cards[i] = hand->cards[j];
		  i++;
	  }
	  j++;
  } 
  
  return ans;
}

int compare_hands(deck_t * hand1, deck_t * hand2) {
	hand_eval_t h1, h2;
	qsort(hand1->cards, hand1->n_cards, sizeof(card_t *), card_ptr_comp);
	qsort(hand2->cards, hand2->n_cards, sizeof(card_t *), card_ptr_comp);
	h1 = evaluate_hand(hand1);
	h2 = evaluate_hand(hand2);

	if(h1.ranking == h2.ranking){
		for(size_t i = 0; i < 5; i++){
			if(h1.cards[i]->value > h2.cards[i]->value)
				return 1;
			else if(h1.cards[i]->value < h2.cards[i]->value)
				return -1;
		}
		// all card values are same
		return 0;		
	}
	//highest ranking 0, lowest 8
	return h2.ranking - h1.ranking; // +ve if 1 wins,-ve if 2 wins
}



//You will write this function in Course 4.
//For now, we leave a prototype (and provide our
//implementation in eval-c4.o) so that the
//other functions we have provided can make
//use of get_match_counts.
void add_one_match_count(unsigned *array, size_t idx, size_t counts) {
  for(size_t i = idx; i < idx + counts; i++){
    array[i] = (unsigned) counts;
  }
}

unsigned * get_match_counts(deck_t * hand) {
  unsigned * ans = malloc(hand->n_cards * sizeof(*ans));
  size_t one_match_count = 1;
  size_t ans_idx = 0;
  for(size_t i = 1; i < hand->n_cards; i++){
    if(hand->cards[i]->value == hand->cards[i - 1]->value){
      one_match_count++;
    }
    else{
      add_one_match_count(ans, ans_idx, one_match_count);
      ans_idx += one_match_count;
      one_match_count = 1;
    }
  }
  if(ans_idx != hand->n_cards){
    add_one_match_count(ans, ans_idx, one_match_count);
  }
  return ans;
}

// We provide the below functions.  You do NOT need to modify them
// In fact, you should not modify them!


//This function copies a straight starting at index "ind" from deck "from".
//This copies "count" cards (typically 5).
//into the card array "to"
//if "fs" is NUM_SUITS, then suits are ignored.
//if "fs" is any other value, a straight flush (of that suit) is copied.
void copy_straight(card_t ** to, deck_t *from, size_t ind, suit_t fs, size_t count) {
  assert(fs == NUM_SUITS || from->cards[ind]->suit == fs);
  unsigned nextv = from->cards[ind]->value;
  size_t to_ind = 0;
  while (count > 0) {
    assert(ind < from->n_cards);
    assert(nextv >= 2);
    assert(to_ind <5);
    if (from->cards[ind]->value == nextv &&
	(fs == NUM_SUITS || from->cards[ind]->suit == fs)){
      to[to_ind] = from->cards[ind];
      to_ind++;
      count--;
      nextv--;
    }
    ind++;
  }
}


//This looks for a straight (or straight flush if "fs" is not NUM_SUITS)
// in "hand".  It calls the student's is_straight_at for each possible
// index to do the work of detecting the straight.
// If one is found, copy_straight is used to copy the cards into
// "ans".
int find_straight(deck_t * hand, suit_t fs, hand_eval_t * ans) {
  if (hand->n_cards < 5){
    return 0;
  }
  for(size_t i = 0; i <= hand->n_cards -5; i++) {
    int x = is_straight_at(hand, i, fs);
    if (x != 0){
      if (x < 0) { //ace low straight
	assert(hand->cards[i]->value == VALUE_ACE &&
	       (fs == NUM_SUITS || hand->cards[i]->suit == fs));
	ans->cards[4] = hand->cards[i];
	size_t cpind = i+1;
	while(hand->cards[cpind]->value != 5 ||
	      !(fs==NUM_SUITS || hand->cards[cpind]->suit ==fs)){
	  cpind++;
	  assert(cpind < hand->n_cards);
	}
	copy_straight(ans->cards, hand, cpind, fs,4) ;
      }
      else {
	copy_straight(ans->cards, hand, i, fs,5);
      }
      return 1;
    }
  }
  return 0;
}


//This function puts all the hand evaluation logic together.
//This function is longer than we generally like to make functions,
//and is thus not so great for readability :(
hand_eval_t evaluate_hand(deck_t * hand) {
  suit_t fs = flush_suit(hand);
  hand_eval_t ans;
  if (fs != NUM_SUITS) {
    if(find_straight(hand, fs, &ans)) {
      ans.ranking = STRAIGHT_FLUSH;
      return ans;
    }
  }
  unsigned * match_counts = get_match_counts(hand);
  unsigned n_of_a_kind = get_largest_element(match_counts, hand->n_cards);
  assert(n_of_a_kind <= 4);
  size_t match_idx = get_match_index(match_counts, hand->n_cards, n_of_a_kind);
  ssize_t other_pair_idx = find_secondary_pair(hand, match_counts, match_idx);
  free(match_counts);
  if (n_of_a_kind == 4) { //4 of a kind
    return build_hand_from_match(hand, 4, FOUR_OF_A_KIND, match_idx);
  }
  else if (n_of_a_kind == 3 && other_pair_idx >= 0) {     //full house
    ans = build_hand_from_match(hand, 3, FULL_HOUSE, match_idx);
    ans.cards[3] = hand->cards[other_pair_idx];
    ans.cards[4] = hand->cards[other_pair_idx+1];
    return ans;
  }
  else if(fs != NUM_SUITS) { //flush
    ans.ranking = FLUSH;
    size_t copy_idx = 0;
    for(size_t i = 0; i < hand->n_cards;i++) {
      if (hand->cards[i]->suit == fs){
	ans.cards[copy_idx] = hand->cards[i];
	copy_idx++;
	if (copy_idx >=5){
	  break;
	}
      }
    }
    return ans;
  }
  else if(find_straight(hand,NUM_SUITS, &ans)) {     //straight
    ans.ranking = STRAIGHT;
    return ans;
  }
  else if (n_of_a_kind == 3) { //3 of a kind
    return build_hand_from_match(hand, 3, THREE_OF_A_KIND, match_idx);
  }
  else if (other_pair_idx >=0) {     //two pair
    assert(n_of_a_kind ==2);
    ans = build_hand_from_match(hand, 2, TWO_PAIR, match_idx);
    ans.cards[2] = hand->cards[other_pair_idx];
    ans.cards[3] = hand->cards[other_pair_idx + 1];
    if (match_idx > 0) {
      ans.cards[4] = hand->cards[0];
    }
    else if (other_pair_idx > 2) {  //if match_idx is 0, first pair is in 01
      //if other_pair_idx > 2, then, e.g. A A K Q Q
      ans.cards[4] = hand->cards[2];
    }
    else {       //e.g., A A K K Q
      ans.cards[4] = hand->cards[4];
    }
    return ans;
  }
  else if (n_of_a_kind == 2) {
    return build_hand_from_match(hand, 2, PAIR, match_idx);
  }
  return build_hand_from_match(hand, 0, NOTHING, 0);
}
