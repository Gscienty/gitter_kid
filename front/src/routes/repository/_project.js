import React from 'react';
import { Menu, Icon, Row, Col } from 'antd';
import BaseComponent from '../../components/BasePage'

export default class ProjectTemplate extends BaseComponent {
    unit() { }

    page() {
        return <div>
            <Row type="flex" justify="center">
                <Col lg={ 20 } sm={ 24 }>
                    <h2>{ this.props.match.params.repositoriesName } / { this.props.match.params.repositoryName }</h2>
                    <Menu onClick={ this.handleMenuOnClick.bind(this) } mode="horizontal" selectedKeys={[ this.props.projectEntryUnit ]}>
                        <Menu.Item key="code"><Icon type="code-o" />Code</Menu.Item>
                    </Menu>
                </Col>
            </Row>

            <Row type="flex" justify="center" align="middle">
                <Col lg={ 20 } sm={ 24 }>
                    { this.unit() }
                </Col>
            </Row>
        </div>
    }
}