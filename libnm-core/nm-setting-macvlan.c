// SPDX-License-Identifier: LGPL-2.1+
/*
 * Copyright (C) 2015 Red Hat, Inc.
 */

#include "nm-default.h"

#include "nm-setting-macvlan.h"

#include <stdlib.h>

#include "nm-utils.h"
#include "nm-setting-connection.h"
#include "nm-setting-private.h"
#include "nm-setting-wired.h"
#include "nm-connection-private.h"

/**
 * SECTION:nm-setting-macvlan
 * @short_description: Describes connection properties for macvlan interfaces
 *
 * The #NMSettingMacvlan object is a #NMSetting subclass that describes properties
 * necessary for connection to macvlan interfaces.
 **/

/*****************************************************************************/

NM_GOBJECT_PROPERTIES_DEFINE_BASE (
	PROP_PARENT,
	PROP_MODE,
	PROP_PROMISCUOUS,
	PROP_TAP,
);

typedef struct {
	char *parent;
	NMSettingMacvlanMode mode;
	gboolean promiscuous;
	gboolean tap;
} NMSettingMacvlanPrivate;

G_DEFINE_TYPE (NMSettingMacvlan, nm_setting_macvlan, NM_TYPE_SETTING)

#define NM_SETTING_MACVLAN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), NM_TYPE_SETTING_MACVLAN, NMSettingMacvlanPrivate))

/*****************************************************************************/

/**
 * nm_setting_macvlan_get_parent:
 * @setting: the #NMSettingMacvlan
 *
 * Returns: the #NMSettingMacvlan:parent property of the setting
 *
 * Since: 1.2
 **/
const char *
nm_setting_macvlan_get_parent (NMSettingMacvlan *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_MACVLAN (setting), NULL);
	return NM_SETTING_MACVLAN_GET_PRIVATE (setting)->parent;
}

/**
 * nm_setting_macvlan_get_mode:
 * @setting: the #NMSettingMacvlan
 *
 * Returns: the #NMSettingMacvlan:mode property of the setting
 *
 * Since: 1.2
 **/
NMSettingMacvlanMode
nm_setting_macvlan_get_mode (NMSettingMacvlan *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_MACVLAN (setting), NM_SETTING_MACVLAN_MODE_UNKNOWN);
	return NM_SETTING_MACVLAN_GET_PRIVATE (setting)->mode;
}

/**
 * nm_setting_macvlan_get_promiscuous:
 * @setting: the #NMSettingMacvlan
 *
 * Returns: the #NMSettingMacvlan:promiscuous property of the setting
 *
 * Since: 1.2
 **/
gboolean
nm_setting_macvlan_get_promiscuous (NMSettingMacvlan *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_MACVLAN (setting), FALSE);
	return NM_SETTING_MACVLAN_GET_PRIVATE (setting)->promiscuous;
}

/**
 * nm_setting_macvlan_get_tap:
 * @setting: the #NMSettingMacvlan
 *
 * Returns: the #NMSettingMacvlan:tap property of the setting
 *
 * Since: 1.2
 **/
gboolean
nm_setting_macvlan_get_tap (NMSettingMacvlan *setting)
{
	g_return_val_if_fail (NM_IS_SETTING_MACVLAN (setting), FALSE);
	return NM_SETTING_MACVLAN_GET_PRIVATE (setting)->tap;
}

/*****************************************************************************/

static gboolean
verify (NMSetting *setting, NMConnection *connection, GError **error)
{
	NMSettingMacvlanPrivate *priv = NM_SETTING_MACVLAN_GET_PRIVATE (setting);
	NMSettingWired *s_wired;

	if (connection)
		s_wired = nm_connection_get_setting_wired (connection);
	else
		s_wired = NULL;

	if (priv->parent) {
		if (   !nm_utils_is_uuid (priv->parent)
		    && !nm_utils_ifname_valid_kernel (priv->parent, NULL)) {
			g_set_error (error,
			             NM_CONNECTION_ERROR,
			             NM_CONNECTION_ERROR_INVALID_PROPERTY,
			             _("'%s' is neither an UUID nor an interface name"),
			             priv->parent);
			g_prefix_error (error, "%s.%s: ", NM_SETTING_MACVLAN_SETTING_NAME, NM_SETTING_MACVLAN_PARENT);
			return FALSE;
		}
	} else {
		/* If parent is NULL, the parent must be specified via
		 * NMSettingWired:mac-address.
		 */
		if (   connection
		    && (!s_wired || !nm_setting_wired_get_mac_address (s_wired))) {
			g_set_error (error,
			             NM_CONNECTION_ERROR,
			             NM_CONNECTION_ERROR_MISSING_PROPERTY,
			             _("property is not specified and neither is '%s:%s'"),
			             NM_SETTING_WIRED_SETTING_NAME, NM_SETTING_WIRED_MAC_ADDRESS);
			g_prefix_error (error, "%s.%s: ", NM_SETTING_MACVLAN_SETTING_NAME, NM_SETTING_MACVLAN_PARENT);
			return FALSE;
		}
	}

	if (!priv->promiscuous && priv->mode != NM_SETTING_MACVLAN_MODE_PASSTHRU) {
		g_set_error_literal (error,
		                     NM_CONNECTION_ERROR,
		                     NM_CONNECTION_ERROR_INVALID_PROPERTY,
		                     _("non promiscuous operation is allowed only in passthru mode"));
		g_prefix_error (error, "%s.%s: ",
		                NM_SETTING_MACVLAN_SETTING_NAME,
		                NM_SETTING_MACVLAN_PROMISCUOUS);
		return FALSE;
	}

	return TRUE;
}

/*****************************************************************************/

static void
get_property (GObject *object, guint prop_id,
              GValue *value, GParamSpec *pspec)
{
	NMSettingMacvlan *setting = NM_SETTING_MACVLAN (object);
	NMSettingMacvlanPrivate *priv = NM_SETTING_MACVLAN_GET_PRIVATE (setting);

	switch (prop_id) {
	case PROP_PARENT:
		g_value_set_string (value, priv->parent);
		break;
	case PROP_MODE:
		g_value_set_uint (value, priv->mode);
		break;
	case PROP_PROMISCUOUS:
		g_value_set_boolean (value, priv->promiscuous);
		break;
	case PROP_TAP:
		g_value_set_boolean (value, priv->tap);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
set_property (GObject *object, guint prop_id,
              const GValue *value, GParamSpec *pspec)
{
	NMSettingMacvlan *setting = NM_SETTING_MACVLAN (object);
	NMSettingMacvlanPrivate *priv = NM_SETTING_MACVLAN_GET_PRIVATE (setting);

	switch (prop_id) {
	case PROP_PARENT:
		g_free (priv->parent);
		priv->parent = g_value_dup_string (value);
		break;
	case PROP_MODE:
		priv->mode = g_value_get_uint (value);
		break;
	case PROP_PROMISCUOUS:
		priv->promiscuous = g_value_get_boolean (value);
		break;
	case PROP_TAP:
		priv->tap = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

/*****************************************************************************/

static void
nm_setting_macvlan_init (NMSettingMacvlan *setting)
{
}

/**
 * nm_setting_macvlan_new:
 *
 * Creates a new #NMSettingMacvlan object with default values.
 *
 * Returns: (transfer full): the new empty #NMSettingMacvlan object
 *
 * Since: 1.2
 **/
NMSetting *
nm_setting_macvlan_new (void)
{
	return (NMSetting *) g_object_new (NM_TYPE_SETTING_MACVLAN, NULL);
}

static void
finalize (GObject *object)
{
	NMSettingMacvlan *setting = NM_SETTING_MACVLAN (object);
	NMSettingMacvlanPrivate *priv = NM_SETTING_MACVLAN_GET_PRIVATE (setting);

	g_free (priv->parent);

	G_OBJECT_CLASS (nm_setting_macvlan_parent_class)->finalize (object);
}

static void
nm_setting_macvlan_class_init (NMSettingMacvlanClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	NMSettingClass *setting_class = NM_SETTING_CLASS (klass);

	g_type_class_add_private (klass, sizeof (NMSettingMacvlanPrivate));

	object_class->get_property = get_property;
	object_class->set_property = set_property;
	object_class->finalize     = finalize;

	setting_class->verify = verify;

	/**
	 * NMSettingMacvlan:parent:
	 *
	 * If given, specifies the parent interface name or parent connection UUID
	 * from which this MAC-VLAN interface should be created.  If this property is
	 * not specified, the connection must contain an #NMSettingWired setting
	 * with a #NMSettingWired:mac-address property.
	 *
	 * Since: 1.2
	 **/
	obj_properties[PROP_PARENT] =
	    g_param_spec_string (NM_SETTING_MACVLAN_PARENT, "", "",
	                         NULL,
	                         G_PARAM_READWRITE |
	                         G_PARAM_CONSTRUCT |
	                         NM_SETTING_PARAM_INFERRABLE |
	                         G_PARAM_STATIC_STRINGS);

	/**
	 * NMSettingMacvlan:mode:
	 *
	 * The macvlan mode, which specifies the communication mechanism between multiple
	 * macvlans on the same lower device.
	 *
	 * Since: 1.2
	 **/
	obj_properties[PROP_MODE] =
	    g_param_spec_uint (NM_SETTING_MACVLAN_MODE, "", "",
	                       0, G_MAXUINT, 0,
	                       G_PARAM_READWRITE |
	                       G_PARAM_CONSTRUCT |
	                       NM_SETTING_PARAM_INFERRABLE |
	                       G_PARAM_STATIC_STRINGS);

	/**
	 * NMSettingMacvlan:promiscuous:
	 *
	 * Whether the interface should be put in promiscuous mode.
	 *
	 * Since: 1.2
	 **/
	obj_properties[PROP_PROMISCUOUS] =
	    g_param_spec_boolean (NM_SETTING_MACVLAN_PROMISCUOUS, "", "",
	                          TRUE,
	                          G_PARAM_READWRITE |
	                          G_PARAM_CONSTRUCT |
	                          NM_SETTING_PARAM_INFERRABLE |
	                          G_PARAM_STATIC_STRINGS);

	/**
	 * NMSettingMacvlan:tap:
	 *
	 * Whether the interface should be a MACVTAP.
	 *
	 * Since: 1.2
	 **/
	obj_properties[PROP_TAP] =
	    g_param_spec_boolean (NM_SETTING_MACVLAN_TAP, "", "",
	                          FALSE,
	                          G_PARAM_READWRITE |
	                          G_PARAM_CONSTRUCT |
	                          NM_SETTING_PARAM_INFERRABLE |
	                          G_PARAM_STATIC_STRINGS);

	g_object_class_install_properties (object_class, _PROPERTY_ENUMS_LAST, obj_properties);

	_nm_setting_class_commit (setting_class, NM_META_SETTING_TYPE_MACVLAN);
}
