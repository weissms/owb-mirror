/*
 * Copyright (C) 2007 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <iostream>

#include "config.h"
#include "BTLogHelper.h"

#include "BALConfiguration.h"
#include "BIEvent.h"
#include "BIEventLoop.h"
#include "BIGraphicsDevice.h"
#include "BIKeyboardEvent.h"
#include "BIWindow.h"
//#include "BIWindowManager.h"
#include "../TestManager/TestManager.h"

using namespace BAL;

typedef enum
{
  None,
  VKTest, // test VKs
  KeyIdentifierTest, // Test Key Identifier
  ModifierTest // Test modifier
} kTestType;

typedef enum
{
  NONE = 0x00,
  SHIFT = 0x01,
  CTRL = 0x02,
  ALT = 0x04
} kTestModifier;

typedef struct
{
  const char* msKey;
  kTestType mType;
  int mVK;
  int mModifier;
} QuestionStruct;

static QuestionStruct gVKQuestionArray[] = {
  { "Left Arrow", VKTest, BAL::BIKeyboardEvent::VK_LEFT, 0 },
  { "Right Arrow", VKTest, BAL::BIKeyboardEvent::VK_RIGHT, 0 },
  { "Down Arrow", VKTest, BAL::BIKeyboardEvent::VK_DOWN, 0 },
  { "Up Arrow", VKTest, BAL::BIKeyboardEvent::VK_UP, 0 },
  { "a", VKTest, BAL::BIKeyboardEvent::VK_A, 0 },
  { NULL, None, 0, 0 }
};

static QuestionStruct gLettersQuestionArray[] = {
  { "a", KeyIdentifierTest, 0 ,0 },
  { "A", KeyIdentifierTest, 0 ,0 },
  { "h", KeyIdentifierTest, 0 ,0 },
  { "H", KeyIdentifierTest, 0 ,0 },
  { "�", KeyIdentifierTest, 0 ,0 },
  { "�", KeyIdentifierTest, 0 ,0 },
  { NULL, None, 0, 0 }
};

static QuestionStruct gModifierQuestionArray[] = {
  { "a", ModifierTest, BAL::BIKeyboardEvent::VK_A,  NONE },
  { "ctrl b", ModifierTest, BAL::BIKeyboardEvent::VK_B, CTRL },
  { "alt c", ModifierTest, BAL::BIKeyboardEvent::VK_C, ALT },
  { "shift d", ModifierTest, BAL::BIKeyboardEvent::VK_D, SHIFT },
  { "ctrl shift e", ModifierTest, BAL::BIKeyboardEvent::VK_E, CTRL | SHIFT },
  { "ctrl alt f", ModifierTest, BAL::BIKeyboardEvent::VK_F, CTRL | ALT },
  { "shift alt g", ModifierTest, BAL::BIKeyboardEvent::VK_G, SHIFT | ALT },
  { "ctrl shift alt h", ModifierTest, BAL::BIKeyboardEvent::VK_H, CTRL | SHIFT | ALT },
  { NULL, None, 0, 0 }
};

class InputEventTestInteractif {
public:
  /**
   * Purpose of this test is to ask user to press all keys,
   * with all modes if they exists (capslock on and off, keypad on and off, etc).
   * Test will check that keyIdentifier is non null. User should check that
   * identifier matches the key.
   */
  void TestKeyIdentifier()
  {
    if( !Init() )
      return;

    DBG("Press all keys, one by one.\n");
    DBG("Don't forget to activate and desactivate all modes (CAPSLOCK, KEYPAD, and others if any)\n");
    DBG("Test will display for each key the \"keyIdentifier\".\n");
    DBG("It's the key value for letters and numbers, or a text describing the key.\n");
    DBG("You are requested to check the correct value.\n");
    DBG("Test will validate only that key identifier is not null.\n");
    DBG("Press Q to quit test.\n");

    CheckKeyIdentifier();

    End();
  }

  /**
   * Purpose of this test is to check Virtual Keys (VKs). User will be asked
   * to press main VKs (arrows, function keys). Test checks correct answer.
   */
  void TestVKs()
  {
    if( !Init() )
      return;

    Check( gVKQuestionArray );

    End();
  }

  /**
   * Purpose of this test is to check letters input, lower, upper,
   * and internationalization.
   */
  void TestLetters()
  {
    if( !Init() )
      return;

    Check( gLettersQuestionArray );

    End();
  }

  /**
   * Purpose of this test is to check modifier keys connections.
   */
  void TestModifierKeys()
  {
    if( !Init() )
      return;

    DBG("This test checks modifiers (ctrl, shift, alt)\n");
    DBG("In this test, you will be asked to press for instance CTRL-a.\n");
    DBG("It means that you press CTRL, keep it pressed, and press a\n");
    DBG("Checking is made on key down\n");

    Check( gModifierQuestionArray );

    End();
  }

private:
  bool Init()
  {
      DBG("start\n");

      m_eventLoop = getBIEventLoop();
      TestManager::AssertTrue("Event loop ok", m_eventLoop != NULL);
      if( m_eventLoop == NULL ) {
        return false;
      }

      TestManager::AssertTrue("Event loop correctly initialized", m_eventLoop->isInitialized());
      if( !m_eventLoop->isInitialized() ) {
        m_eventLoop = NULL;
        return false;
      }
    // FIXME window creation has changed, must update test

      getBIGraphicsDevice()->initialize(800, 600, 32);
      //m_window = getBIWindowManager()->openWindow(0, 0, 800, 600);
/*      m_window = createBCWindowBal(0,0,200,201,32);
      TestManager::AssertTrue("window ok", m_window != NULL);
      if( m_window == NULL ) {
        m_eventLoop = NULL;
        return false;
      }
*/
      return true;

   }

   void End()
   {
      //delete m_window;
      m_window = NULL;

      DBG("end\n");
   }

   void CheckKeyIdentifier()
   {
      BIEvent* event = NULL;
      while(1)
      {
          event = NULL;
          bool bValid = m_eventLoop->WaitEvent(event);
          //printf("coucou bValid = %d\n", bValid);
          if( !bValid )
            continue;
          BIKeyboardEvent* aKeyboardEvent = event->queryIsKeyboardEvent();
          if( aKeyboardEvent )
          {
              TestManager::AssertTrue("Key identifier not null", !aKeyboardEvent->keyIdentifier().isEmpty() );
#ifndef NDEBUG
              printf("KeyIdentifier: %s\n", aKeyboardEvent->keyIdentifier().ascii().data() );
#endif
              if( aKeyboardEvent->virtualKeyCode() == BAL::BIKeyboardEvent::VK_Q )
                break;
          }
      }
   }

  void NextQuestion()
  {
    m_question++;
    m_currentQuestionDisplayed = false;
  }

  bool AreQuestionLeft(QuestionStruct* aQuestions) const
  {
    return aQuestions[ m_question ].msKey != NULL;
  }

  /**
   * Check that the keyboard event matches the expected answer.
   * Returns false if keyboard event is not the one to test (case of the modifier,
   * where we expect the letter tp be typed in, to check the associated modifiers).
   */
  bool CheckAnswer( QuestionStruct* aQuestions, const BIKeyboardEvent& aKey )
  {
    switch( aQuestions[ m_question ].mType )
    {
      case VKTest:
      {
        TestManager::AssertTrue("VK as expected",
          aKey.virtualKeyCode() == aQuestions[ m_question ].mVK );
        return true;
      }
      case KeyIdentifierTest:
      {
        TestManager::AssertTrue("key identifier as expected",
          aKey.keyIdentifier() == aQuestions[ m_question ].msKey );
        return true;
      }
      case ModifierTest:
      {
        if( aKey.virtualKeyCode() == aQuestions[ m_question ].mVK )
        {
          // Check modifiers
          TestManager::AssertTrue("SHIFT TEST",
            aKey.shiftKey() == static_cast<bool>(aQuestions[ m_question ].mModifier & SHIFT) );
          TestManager::AssertTrue("CTRL TEST",
            aKey.ctrlKey() == static_cast<bool>(aQuestions[ m_question ].mModifier & CTRL) );
          TestManager::AssertTrue("ALT TEST",
            aKey.altKey() == static_cast<bool>(aQuestions[ m_question ].mModifier & ALT) );
          return true;
        }
        else
        {
          return false;
        }
      }
      default:
      {
          TestManager::AssertTrue("SHOULD NOT PASS HERE",0);
          return false;
      }
    }
  }

  void DisplayQuestion( QuestionStruct* aQuestions )
  {
    if( !m_currentQuestionDisplayed )
    {
      char sMessage[1024];
      sprintf(sMessage, "Press and release '%s'\n", aQuestions[ m_question ].msKey );
      DBG(sMessage);
      m_currentQuestionDisplayed = true;
    }
  }

  void Check( QuestionStruct* aQuestions )
  {
      BIEvent* event;
      m_question = 0;
      m_currentQuestionDisplayed = false;
      while(AreQuestionLeft( aQuestions ))
      {
          event=NULL;
          bool bValid = m_eventLoop->WaitEvent(event);
          if( !bValid )
            continue;

          DisplayQuestion( aQuestions );

          BIKeyboardEvent* aKeyboardEvent = event->queryIsKeyboardEvent();
          if( aKeyboardEvent && !aKeyboardEvent->isKeyUp() )
          {
            bool bStop = CheckAnswer( aQuestions, *aKeyboardEvent );
            if( !bStop )
              continue;
          }
          else
          {
            continue; // It's not a keyboard event, or not on key down. Just ignore it.
          }
          NextQuestion();
      }
  }

private:
  BIWindow* m_window;
  BIEventLoop* m_eventLoop;

  int m_question;
  bool m_currentQuestionDisplayed;
};

class TestSuiteEvents
{
public:
  static void TestKeyIdentifier() {
    InputEventTestInteractif a;
    a.TestKeyIdentifier();
  }
  static void TestVKs() {
    InputEventTestInteractif a;
    a.TestVKs();
  }
  static void TestModifierKeys() {
    InputEventTestInteractif a;
    a.TestModifierKeys();
  }
  static void TestLetters() {
    InputEventTestInteractif a;
    a.TestLetters();
  }
};

static TestNode TestKeyIdentifier = { "TestKeyIdentifier", "test keys identifiers",
  TestNode::FUNCTIONALY_INTERACTIVE, TestSuiteEvents::TestKeyIdentifier, NULL };

static TestNode TestVKs = {
    "TestVKs",
    "test virtual key codes",
    TestNode::FUNCTIONALY_INTERACTIVE,
    TestSuiteEvents::TestVKs,
		NULL
};

static TestNode TestModifierKeys = {
    "TestModifiers",
    "test modifiers (shift, ctrl, alt, ...)",
    TestNode::FUNCTIONALY_INTERACTIVE,
    TestSuiteEvents::TestModifierKeys,
		NULL
};

static TestNode TestLetters = {
    "TestKeyIdentifier",
    "test keys identifiers",
    TestNode::FUNCTIONALY_INTERACTIVE,
    TestSuiteEvents::TestLetters,
		NULL
};

TestNode* gEventTestNodeList[] = {
  &TestKeyIdentifier,
  &TestVKs,
  &TestModifierKeys,
  &TestLetters,
	NULL
};

TestNode gTestSuiteEvent = {
    "TestEvent",
    "test events",
    TestNode::TEST_SUITE,
    NULL, /* no function, it's a test suite */
		gEventTestNodeList
};
