#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <functional>
#include <unordered_set>
#include <algorithm>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/range/algorithm.hpp>

using uint = uint32_t;
using Boggle_results = std::vector<std::string>;
using Boggle_idxed_word = std::vector<uint>;
using Point = boost::geometry::model::d2::point_xy<long>;

class Board
{
  public:
    using Board_val = std::array<char, 16>;
    static const uint width = 4;
    static const uint height = 4;
    static const uint get_y(const uint indx) { return indx / width; }                  // y = index / width
    static const uint get_x(const uint indx) { return indx - (indx / width) * width; } // x = (index - y*width)
    static const uint get_index(const uint x, const uint y) { return y * width + x; }  // index = x*width + y
    static const std::vector<uint> get_neighbours(const uint indx) {
        static const std::vector<Point> directions {
            {0, -1},{-1, -1},{-1, 0},{-1, 1},{0, 1},{1, 1},{1, 0},{1, -1},
        };
        std::vector<uint> res;
        Point cur { get_x(indx), get_y(indx) };
        for (auto dir : directions) {
            Point d = {dir.x() + cur.x(), dir.y() + cur.y()};
            if (d.x() < width && d.x() >= 0
             && d.y() < height && d.y() >= 0) {
                res.push_back(get_index(d.x(), d.y()));
            }
        }
        return res;
    };
    const Board_val val = {
        'a', 'b', 'c', 'd',
        'e', 'f', 'g', 'h',
        'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p',
    };
};

std::string indxed_word_to_str(const Boggle_idxed_word &w, const Board &b) {
    std::string r(w.size(), '\0');
    boost::range::transform(w, r.begin(), [&b](uint idx) {
        return b.val[idx];
    });
    return r;
}

class Dict {
public:
    virtual bool is_a_word(const std::string &s) const = 0;
    virtual bool is_start_of_a_word(const std::string &s) const = 0;
};

class TrueDict : public Dict {
public:
    bool is_a_word(const std::string &s) const override { return true; }
    bool is_start_of_a_word(const std::string &s) const override { return true; }
};

Boggle_results solve_boggle(const Boggle_idxed_word &w, const Board &b, const Dict &d = TrueDict()) {
    Boggle_results res;

    auto str = indxed_word_to_str(w, b);
    if (!d.is_start_of_a_word(str))
        return res;

    for (auto dir : b.get_neighbours(*w.rbegin())) {
        if (boost::range::find(w, dir) == w.end()) {
            Boggle_idxed_word n(w);
            n.push_back(dir);
            boost::range::copy(solve_boggle(n, b, d), std::back_inserter(res));
        }
    }

    if (d.is_a_word(str))
        res.push_back(str);

    return res;
}

Boggle_results solve_boggle(const Dict &d = TrueDict()) {
    const Board b;
    Boggle_results res;
    for (auto i = 0u; i < b.width*b.height; i++) {
        boost::range::copy(solve_boggle({i}, b, d), std::back_inserter(res));
    }
    return res;
}

class TestDict : public Dict {
    const std::string w = "abcdef";
public:
    bool is_a_word(const std::string &s) const override { return s == w; }
    bool is_start_of_a_word(const std::string &s) const override { return s == w.substr(0, s.size()); }
};

class BoggleDict : public Dict {
    std::unordered_set<std::string> dict;
public:
    BoggleDict() {
        dict = {
            "a", "ability", "able", "about", "above", "accept", "according", "account", "across", "act", "action", "activity", "actually", "add", "address", "administration", "admit", "adult", "affect", "after", "again", "against", "age", "agency", "agent", "ago", "agree", "agreement", "ahead", "air", "all", "allow", "almost", "alone", "along", "already", "also", "although", "always", "American", "among", "amount", "analysis", "and", "animal", "another", "answer", "any", "anyone", "anything", "appear", "apply", "approach", "area", "argue", "arm", "around", "arrive", "art", "article", "artist", "as", "ask", "assume", "at", "attack", "attention", "attorney", "audience", "author", "authority", "available", "avoid", "away", "baby", "back", "bad", "bag", "ball", "bank", "bar", "base", "be", "beat", "beautiful", "because", "become", "bed", "before", "begin", "behavior", "behind", "believe", "benefit", "best", "better", "between", "beyond", "big", "bill", "billion", "bit", "black", "blood", "blue", "board", "body", "book", "born", "both", "box", "boy", "break", "bring", "brother", "budget", "build", "building", "business", "but", "buy", "by", "call", "camera", "campaign", "can", "cancer", "candidate", "capital", "car", "card", "care", "career", "carry", "case", "catch", "cause", "cell", "center", "central", "century", "certain", "certainly", "chair", "challenge", "chance", "change", "character", "charge", "check", "child", "choice", "choose", "church", "citizen", "city", "civil", "claim", "class", "clear", "clearly", "close", "coach", "cold", "collection", "college", "color", "come", "commercial", "common", "community", "company", "compare", "computer", "concern", "condition", "conference", "Congress", "consider", "consumer", "contain", "continue", "control", "cost", "could", "country", "couple", "course", "court", "cover", "create", "crime", "cultural", "culture", "cup", "current", "customer", "cut", "dark", "data", "daughter", "day", "dead", "deal", "death", "debate", "decade", "decide", "decision", "deep", "defense", "degree", "Democrat", "democratic", "describe", "design", "despite", "detail", "determine", "develop", "development", "die", "difference", "different", "difficult", "dinner", "direction", "director", "discover", "discuss", "discussion", "disease", "do", "doctor", "dog", "door", "down", "draw", "dream", "drive", "drop", "drug", "during", "each", "early", "east", "easy", "eat", "economic", "economy", "edge", "education", "effect", "effort", "eight", "either", "election", "else", "employee", "end", "energy", "enjoy", "enough", "enter", "entire", "environment", "environmental", "especially", "establish", "even", "evening", "event", "ever", "every", "everybody", "everyone", "everything", "evidence", "exactly", "example", "executive", "exist", "expect", "experience", "expert", "explain", "eye", "face", "fact", "factor", "fail", "fall", "family", "far", "fast", "father", "fear", "federal", "feel", "feeling", "few", "field", "fight", "figure", "fill", "film", "final", "finally", "financial", "find", "fine", "finger", "finish", "fire", "firm", "first", "fish", "five", "floor", "fly", "focus", "follow", "food", "foot", "for", "force", "foreign", "forget", "form", "former", "forward", "four", "free", "friend", "from", "front", "full", "fund", "future", "game", "garden", "gas", "general", "generation", "get", "girl", "give", "glass", "go", "goal", "good", "government", "great", "green", "ground", "group", "grow", "growth", "guess", "gun", "guy", "hair", "half", "hand", "hang", "happen", "happy", "hard", "have", "he", "head", "health", "hear", "heart", "heat", "heavy", "help", "her", "here", "herself", "high", "him", "himself", "his", "history", "hit", "hold", "home", "hope", "hospital", "hot", "hotel", "hour", "house", "how", "however", "huge", "human", "hundred", "husband", "I", "idea", "identify", "if", "image", "imagine", "impact", "important", "improve", "in", "include", "including", "increase", "indeed", "indicate", "individual", "industry", "information", "inside", "instead", "institution", "interest", "interesting", "international", "interview", "into", "investment", "involve", "issue", "it", "item", "its", "itself", "job", "join", "just", "keep", "key", "kid", "kill", "kind", "kitchen", "know", "knowledge", "land", "language", "large", "last", "late", "later", "laugh", "law", "lawyer", "lay", "lead", "leader", "learn", "least", "leave", "left", "leg", "legal", "less", "let", "letter", "level", "lie", "life", "light", "like", "likely", "line", "list", "listen", "little", "live", "local", "long", "look", "lose", "loss", "lot", "love", "low", "machine", "magazine", "main", "maintain", "major", "majority", "make", "man", "manage", "management", "manager", "many", "market", "marriage", "material", "matter", "may", "maybe", "me", "mean", "measure", "media", "medical", "meet", "meeting", "member", "memory", "mention", "message", "method", "middle", "might", "military", "million", "mind", "minute", "miss", "mission", "model", "modern", "moment", "money", "month", "more", "morning", "most", "mother", "mouth", "move", "movement", "movie", "Mr", "Mrs", "much", "music", "must", "my", "myself", "name", "nation", "national", "natural", "nature", "near", "nearly", "necessary", "need", "network", "never", "new", "news", "newspaper", "next", "nice", "night", "no", "none", "nor", "north", "not", "note", "nothing", "notice", "now", "n't", "number", "occur", "of", "off", "offer", "office", "officer", "official", "often", "oh", "oil", "ok", "old", "on", "once", "one", "only", "onto", "open", "operation", "opportunity", "option", "or", "order", "organization", "other", "others", "our", "out", "outside", "over", "own", "owner", "page", "pain", "painting", "paper", "parent", "part", "participant", "particular", "particularly", "partner", "party", "pass", "past", "patient", "pattern", "pay", "peace", "people", "per", "perform", "performance", "perhaps", "period", "person", "personal", "phone", "physical", "pick", "picture", "piece", "place", "plan", "plant", "play", "player", "PM", "point", "police", "policy", "political", "politics", "poor", "popular", "population", "position", "positive", "possible", "power", "practice", "prepare", "present", "president", "pressure", "pretty", "prevent", "price", "private", "probably", "problem", "process", "produce", "product", "production", "professional", "professor", "program", "project", "property", "protect", "prove", "provide", "public", "pull", "purpose", "push", "put", "quality", "question", "quickly", "quite", "race", "radio", "raise", "range", "rate", "rather", "reach", "read", "ready", "real", "reality", "realize", "really", "reason", "receive", "recent", "recently", "recognize", "record", "red", "reduce", "reflect", "region", "relate", "relationship", "religious", "remain", "remember", "remove", "report", "represent", "Republican", "require", "research", "resource", "respond", "response", "responsibility", "rest", "result", "return", "reveal", "rich", "right", "rise", "risk", "road", "rock", "role", "room", "rule", "run", "safe", "same", "save", "say", "scene", "school", "science", "scientist", "score", "sea", "season", "seat", "second", "section", "security", "see", "seek", "seem", "sell", "send", "senior", "sense", "series", "serious", "serve", "service", "set", "seven", "several", "sex", "sexual", "shake", "share", "she", "shoot", "short", "shot", "should", "shoulder", "show", "side", "sign", "significant", "similar", "simple", "simply", "since", "sing", "single", "sister", "sit", "site", "situation", "six", "size", "skill", "skin", "small", "smile", "so", "social", "society", "soldier", "some", "somebody", "someone", "something", "sometimes", "son", "song", "soon", "sort", "sound", "source", "south", "southern", "space", "speak", "special", "specific", "speech", "spend", "sport", "spring", "staff", "stage", "stand", "standard", "star", "start", "state", "statement", "station", "stay", "step", "still", "stock", "stop", "store", "story", "strategy", "street", "strong", "structure", "student", "study", "stuff", "style", "subject", "success", "successful", "such", "suddenly", "suffer", "suggest", "summer", "support", "sure", "surface", "system", "table", "take", "talk", "task", "tax", "teach", "teacher", "team", "technology", "television", "tell", "ten", "tend", "term", "test", "than", "thank", "that", "the", "their", "them", "themselves", "then", "theory", "there", "these", "they", "thing", "think", "third", "this", "those", "though", "thought", "thousand", "threat", "three", "through", "throughout", "throw", "thus", "time", "to", "today", "together", "tonight", "too", "top", "total", "tough", "toward", "town", "trade", "traditional", "training", "travel", "treat", "treatment", "tree", "trial", "trip", "trouble", "true", "truth", "try", "turn", "TV", "two", "type", "under", "understand", "unit", "until", "up", "upon", "us", "use", "usually", "value", "various", "very", "victim", "view", "violence", "visit", "voice", "vote", "wait", "walk", "wall", "want", "war", "watch", "water", "way", "we", "weapon", "wear", "week", "weight", "well", "west", "western", "what", "whatever", "when", "where", "whether", "which", "while", "white", "who", "whole", "whom", "whose", "why", "wide", "wife", "will", "win", "wind", "window", "wish", "with", "within", "without", "woman", "wonder", "word", "work", "worker", "world", "worry", "would", "write", "writer", "wrong", "yard", "yeah", "year", "yes", "yet", "you", "young", "your", "yourself",
            "knife", "plonk", "bein", "fink", "find", "glop", "jink", "knop", "koji", "mink", "mino", "nief", "nife", "onie", "polk", "ponk",
        };
    }
    bool is_a_word(const std::string &s) const override { return dict.find(s) != dict.end(); }
    bool is_start_of_a_word(const std::string &s) const override {
        return boost::algorithm::any_of(dict, [&s](const std::string &w){
            return w.substr(0, s.size()) == s;
        });
    }
};

#define MAKE_TEST(TEST, XPCT) std::make_tuple( long(TEST), long(XPCT), (#TEST) )
bool test_on_success() {
    auto fixtures = {
        MAKE_TEST(Board::get_index(0, 0), 0),
        MAKE_TEST(Board::get_index(1, 0), 1),
        MAKE_TEST(Board::get_index(2, 0), 2),
        MAKE_TEST(Board::get_index(3, 0), 3),
        MAKE_TEST(Board::get_index(0, 1), 4),
        MAKE_TEST(Board::get_index(1, 1), 5),
        MAKE_TEST(Board::get_x(0), 0),
        MAKE_TEST(Board::get_x(1), 1),
        MAKE_TEST(Board::get_x(3), 3),
        MAKE_TEST(Board::get_x(4), 0),
        MAKE_TEST(Board::get_x(5), 1),
        MAKE_TEST(Board::get_x(8), 0),
        MAKE_TEST(Board::get_y(0), 0),
        MAKE_TEST(Board::get_y(1), 0),
        MAKE_TEST(Board::get_y(3), 0),
        MAKE_TEST(Board::get_y(4), 1),
        MAKE_TEST(Board::get_y(5), 1),
        MAKE_TEST(Board::get_y(8), 2),
        MAKE_TEST(Board::get_neighbours(0).size(), 3),
        MAKE_TEST(Board::get_neighbours(0)[0], 4),
        MAKE_TEST(Board::get_neighbours(0)[1], 5),
        MAKE_TEST(Board::get_neighbours(0)[2], 1),
        MAKE_TEST(Board::get_neighbours(14).size(), 5),
        MAKE_TEST(Board::get_neighbours(14)[0], 10),
        MAKE_TEST(Board::get_neighbours(14)[1], 9),
        MAKE_TEST(Board::get_neighbours(14)[2], 13),
        MAKE_TEST(Board::get_neighbours(14)[3], 15),
        MAKE_TEST(Board::get_neighbours(14)[4], 11),
        MAKE_TEST(indxed_word_to_str({0,6,15}, Board()).size(), 3),
        MAKE_TEST(indxed_word_to_str({0,6,15}, Board())[0], 'a'),
        MAKE_TEST(indxed_word_to_str({0,6,15}, Board())[1], 'g'),
        MAKE_TEST(indxed_word_to_str({0,6,15}, Board())[2], 'p'),
        MAKE_TEST(indxed_word_to_str({0,6,15}, Board())[2], 'p'),
        MAKE_TEST(solve_boggle({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14}, Board()).size(), 2),
        MAKE_TEST(solve_boggle({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14}, Board())[0].compare("abcdefghijklmnop"), 0),
        MAKE_TEST(solve_boggle({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14}, Board())[1].compare("abcdefghijklmno"), 0),
        MAKE_TEST(TestDict().is_a_word("abcdef"), true),
        MAKE_TEST(TestDict().is_a_word("gnagna"), false),
        MAKE_TEST(TestDict().is_start_of_a_word("ab"), true),
        MAKE_TEST(TestDict().is_start_of_a_word("ba"), false),
        MAKE_TEST(solve_boggle({0,1,2,3,4}, Board(), TestDict()).size(), 1),
        MAKE_TEST(solve_boggle({0,1,2,3,4}, Board(), TestDict())[0].compare("abcdef"), 0),
        MAKE_TEST(solve_boggle(BoggleDict()).size(), 22),
        //MAKE_TEST(solve_boggle().size(), 12029640), /* if you have compute time to spare :D */
    };
    bool success = true;
    for (auto test : fixtures) {
        auto res = std::get<0>(test);
        auto xpt = std::get<1>(test);
        auto msg = std::get<2>(test);
        if (res != xpt) {
            std::cerr << "[ERROR] in test : " << msg <<
                "\n\treturned:   " << res <<
                "\n\texpected: " << xpt << "\n";
            success = false;
        }
    }
    return success;
}

int main(int argc, char **argv)
{
    if (test_on_success())
    {
        std::cout << "Tests succedeed !\n";
        auto words = solve_boggle(BoggleDict());
        std::cout << "found : " << words.size() << " items :\n";
        for (const auto s : words) {
            std::cout << "- " << s << "\n";
        }
    }
    return 0;
}
