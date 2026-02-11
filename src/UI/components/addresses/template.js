import { html } from "/core/UI.js"
import styles from "./styles.css.js"
import "/UI/components/icon/index.js"
import "/UI/components/context/index.js"
import "/UI/components/button/index.js"
import "/UI/components/select/index.js"
import "/UI/components/geo/index.js"
import "/UI/components/modal/index.js"

export const template = html`
    ${styles}
    <header>
        <ui-context data-key="dictionary.addresses" />
        <nav>
            <ui-icon id="add" data-icon="plus-lg" data-size="md" />
        </nav>
    </header>
    <div id="addresses"></div>
    <form id="address-form">
        <div class="inputs">
            <label>
                <input type="checkbox" name="billingAddress" />
                <ui-context data-key="dictionary.billingAddress" />
            </label>
            <label>
                <input type="checkbox" name="shippingAddress" />
                <ui-context data-key="dictionary.shippingAddress" />
            </label>
        </div>
        <input type="text" name="company" />
        <div class="inputs">
            <input type="text" name="firstName" required="required" />
            <input type="text" name="lastName" required="required" />
        </div>
        <div class="inputs">
            <label>
                <input type="radio" name="gender" value="male" required="required" />
                <ui-context data-key="dictionary.male" />
            </label>
            <label>
                <input type="radio" name="gender" value="female" required="required" />
                <ui-context data-key="dictionary.female" />
            </label>
            <label>
                <input type="radio" name="gender" value="other" required="required" />
                <ui-context data-key="dictionary.other" />
            </label>
        </div>
        <ui-geo />
        <input type="text" name="postalCode" required="required" />
        <input type="text" name="addressLine1" required="required" />
        <input type="text" name="addressLine2" required="required" />
        <input type="email" name="email" />
        <input type="tel" name="phone" required="required" />
        <input type="hidden" name="id" />
        <div class="buttons">
            <ui-button class="full" data-left="/images/icons/house-add.svg" id="save">
                <ui-context data-key="dictionary.save"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/x-lg.svg" id="reset">
                <ui-context data-key="dictionary.reset"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/trash3.svg" id="delete">
                <ui-context data-key="dictionary.delete"/>
            </ui-button>
            <ui-button class="full" data-left="/images/icons/x-lg.svg" id="close">
                <ui-context data-key="dictionary.close" />
            </ui-button>
        </div>
    </form>
    <ui-modal data-header="deletion" class="center" id="deletion">
        <p><ui-context data-key="dictionary.deletionMessage" /></p>
        <div class="buttons">
            <ui-button class="full" data-left="/images/icons/check-lg.svg" id="confirm">
                <ui-context data-key="dictionary.confirm" />
            </ui-button>
            <ui-button class="full" data-left="/images/icons/arrow-left.svg" id="cancel">
                <ui-context data-key="dictionary.cancel" />
            </ui-button>
        </div>
    </ui-modal>
`

export default template