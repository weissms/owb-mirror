/*
 * Copyright (C) 2008 Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include "webkitwebpolicydecision.h"

#include "FrameLoaderTypes.h"
#include "webkitprivate.h"

using namespace WebKit;
using namespace WebCore;

extern "C" {

G_DEFINE_TYPE(WebKitWebPolicyDecision, webkit_web_policy_decision, G_TYPE_OBJECT);

struct _WebKitWebPolicyDecisionPrivate {
    WebKitWebFrame* frame;
    FramePolicyFunction framePolicyFunction;
    gboolean isCancelled;
};

#define WEBKIT_WEB_POLICY_DECISION_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_WEB_POLICY_DECISION, WebKitWebPolicyDecisionPrivate))

static void webkit_web_policy_decision_class_init(WebKitWebPolicyDecisionClass* decisionClass)
{
    g_type_class_add_private(decisionClass, sizeof(WebKitWebPolicyDecisionPrivate));
}

static void webkit_web_policy_decision_init(WebKitWebPolicyDecision* decision)
{
    decision->priv = WEBKIT_WEB_POLICY_DECISION_GET_PRIVATE(decision);
}

WebKitWebPolicyDecision* webkit_web_policy_decision_new(WebKitWebFrame* frame, WebCore::FramePolicyFunction function)
{
    g_return_val_if_fail(frame, NULL);

    WebKitWebPolicyDecision* decision = WEBKIT_WEB_POLICY_DECISION(g_object_new(WEBKIT_TYPE_WEB_POLICY_DECISION, NULL));
    WebKitWebPolicyDecisionPrivate* priv = decision->priv;

    priv->frame = frame;
    priv->framePolicyFunction = function;
    priv->isCancelled = FALSE;

    return decision;
}

/**
 * webkit_web_policy_decision_use
 * @decision: a #WebKitWebPolicyDecision
 *
 * Will send the USE decision to the policy implementer.
 *
 * Since: 1.0.3
 */
void webkit_web_policy_decision_use(WebKitWebPolicyDecision* decision)
{
    g_return_if_fail(WEBKIT_IS_WEB_POLICY_DECISION(decision));

    WebKitWebPolicyDecisionPrivate* priv = decision->priv;

    if (!priv->isCancelled)
        (core(priv->frame)->loader()->*(priv->framePolicyFunction))(WebCore::PolicyUse);
}

/**
 * webkit_web_policy_decision_ignore
 * @decision: a #WebKitWebPolicyDecision
 *
 * Will send the IGNORE decision to the policy implementer.
 *
 * Since: 1.0.3
 */
void webkit_web_policy_decision_ignore(WebKitWebPolicyDecision* decision)
{
    g_return_if_fail(WEBKIT_IS_WEB_POLICY_DECISION(decision));

    WebKitWebPolicyDecisionPrivate* priv = decision->priv;

    if (!priv->isCancelled)
        (core(priv->frame)->loader()->*(priv->framePolicyFunction))(WebCore::PolicyIgnore);
}

/**
 * webkit_web_policy_decision_download
 * @decision: a #WebKitWebPolicyDecision
 *
 * Will send the DOWNLOAD decision to the policy implementer.
 *
 * Since: 1.0.3
 */
void webkit_web_policy_decision_download(WebKitWebPolicyDecision* decision)
{
    g_return_if_fail(WEBKIT_IS_WEB_POLICY_DECISION(decision));

    WebKitWebPolicyDecisionPrivate* priv = decision->priv;

    g_warning("Download is not supported");
    webkit_web_policy_decision_ignore(decision);

    if (!priv->isCancelled)
        (core(priv->frame)->loader()->*(priv->framePolicyFunction))(WebCore::PolicyDownload);
}

void webkit_web_policy_decision_cancel(WebKitWebPolicyDecision* decision)
{
    g_return_if_fail(WEBKIT_IS_WEB_POLICY_DECISION(decision));

    WebKitWebPolicyDecisionPrivate* priv = decision->priv;

    priv->isCancelled = TRUE;
}

}
